// --------------------------------------------------------------------------
//   SmartPeak -- Fast and Accurate CE-, GC- and LC-MS(/MS) Data Processing
// --------------------------------------------------------------------------
// Copyright The SmartPeak Team -- Novo Nordisk Foundation 
// Center for Biosustainability, Technical University of Denmark 2018-2021.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL ANY OF THE AUTHORS OR THE CONTRIBUTING
// INSTITUTIONS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
// OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
// WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
// OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
// ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// --------------------------------------------------------------------------
// $Maintainer: Douglas McCloskey, Bertrand Boudaud $
// $Authors: Douglas McCloskey, Bertrand Boudaud $
// --------------------------------------------------------------------------

#include <SmartPeak/ui/InfoWidget.h>
#include <imgui.h>
#include <imgui_internal.h>
#include <plog/Log.h>
#include <SmartPeak/core/SharedProcessors.h>
#include <implot.h>
#include <math.h>

namespace SmartPeak
{

  void InfoWidget::draw()
  {
    showQuickHelpToolTip("Info");
    
    drawWorkflowStatus();
    drawListOfErrors();
    drawLastRunTime();
    drawErrorMessages();

    ImGui::Separator();

    drawSamples();
    drawTransition();
    drawChromatograms();
    drawSpectrums();

    refresh_needed_ = false;
  }

  void InfoWidget::drawWorkflowStatus()
  {
    if (progress_info_.isRunning())
    {
      drawWorkflowProgress();
    }
    else
    {
      ImGui::Text("Workflow status: done");
    }
  }

  void InfoWidget::drawChromatograms()
  {
    std::ostringstream os;
    if (refresh_needed_)
    {
      number_of_chromatograms_ = 0;
      if (application_handler_.sequenceHandler_.getSequence().size() > 0)
      {
        number_of_chromatograms_ = application_handler_.sequenceHandler_.getSequence().at(0).getRawData().getChromatogramMap().getChromatograms().size();
      }
    }
    os << "Number of chromatograms: " << number_of_chromatograms_;
    ImGui::Text("%s", os.str().c_str());
  }

  void InfoWidget::drawSpectrums()
  {
    std::ostringstream os;
    if (refresh_needed_)
    {
      number_of_spectrums_ = 0;
      if (application_handler_.sequenceHandler_.getSequence().size() > 0)
      {
        number_of_spectrums_ = application_handler_.sequenceHandler_.getSequence().at(0).getRawData().getExperiment().getSpectra().size();
      }
    }
    os << "Number of spectrums: " << number_of_spectrums_;
    ImGui::Text("%s", os.str().c_str());
  }

  void InfoWidget::drawSamples()
  {
    std::ostringstream os;
    if (refresh_needed_)
    {
      number_of_samples_ = application_handler_.sequenceHandler_.getSequence().size();
    }
    os << "Number of samples: " << number_of_samples_;
    ImGui::Text("%s", os.str().c_str());
  }

  void InfoWidget::drawTransition()
  {
    std::ostringstream os;
    if (refresh_needed_)
    {
      number_of_transitions_ = 0;
      if (transitions_)
      {
        number_of_transitions_ = transitions_->dimension(0);
      }
    }
    os << "Number of transitions: " << number_of_transitions_;
    ImGui::Text("%s", os.str().c_str());
  }

  void InfoWidget::drawErrorMessages()
  {
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
    for (const auto& error_message : error_messages_)
    {
      ImGui::Text("%s", error_message.c_str());
    }
    ImGui::PopStyleColor();
  }

  void InfoWidget::drawLastRunTime()
  {
    if (progress_info_.lastRunTime() != std::chrono::steady_clock::duration::zero())
    {
      std::ostringstream os;
      os << "Last time workflow execution: ";
      os << formattedTime(progress_info_.lastRunTime());
      ImGui::Text("%s", os.str().c_str());
    }
  }
   
  void InfoWidget::drawWorkflowProgress()
  {
    if (progress_info_.isRunning())
    {
      // running time
      std::chrono::steady_clock::duration running_time = progress_info_.runningTime();
      std::ostringstream os;
      os << "Running time: ";
      os << formattedTime(running_time);
      ImGui::Text("%s", os.str().c_str());

      // estimated time
      os.str("");
      os.clear();
      os << "Estimated remaining time: ";
      auto estimated_time = progress_info_.estimatedRemainingTime();
      if (estimated_time)
      {
        // Enable this verbose log message to check estimation function accuracy
        // LOGD << std::chrono::duration<double>(estimated_time).count() << "," << std::chrono::duration<double>(running_time).count();
        if ((*estimated_time).count() < 0)
        {
          // dont display negative time
          (*estimated_time) = std::chrono::steady_clock::duration::zero();
        }
        os << formattedTime(*estimated_time);
      }
      else
      {
        os << "please wait...";
      }
      ImGui::Text("%s", os.str().c_str());
      if (ImGui::IsItemHovered())
      {
        ImGui::SetTooltip("This time is only a rough estimation.\nIt will be updated regaluary while workflow is running.");
      }
      
      // progress bar
      float progress = progress_info_.progressValue();
      ImGui::ProgressBar(progress);
      if (ImGui::IsItemHovered())
      {
        ImGui::SetTooltip("The progress bar shows workflow steps completed.\nAs some steps can be longer to execute, it may not reflect remaining time.");
      }

      // application processor steps
      if (ImGui::TreeNode("Running workflow commands"))
      {
        size_t index = 0;
        for (const auto& command : progress_info_.allCommands())
        {
          auto command_tuple = std::make_tuple(index, command);
          if (std::find(progress_info_.runningCommands().begin(), progress_info_.runningCommands().end(), command_tuple) != progress_info_.runningCommands().end())
          {
            // we add 1 to the id of the command since it's the way it is displayed in the workflow window
            ImGui::Text("%c [%lu] %s", "|/-\\"[spinner_counter_ & 3], index + 1, command.c_str());
          }
          else
          {
            ImGui::Text("  [%lu] %s", index + 1, command.c_str());
          }
          index++;
        }
        ImGui::TreePop();
      }

      // running samples
      if (ImGui::TreeNode("Running items"))
      {
        for (const auto& running_sample : progress_info_.runningBatch())
        {
          ImGui::Text("%c %s", "|/-\\"[spinner_counter_ & 3], running_sample.c_str());
        }
        ImGui::TreePop();
      }
      spinner_counter_++;
    }
  }

  void InfoWidget::drawListOfErrors()
  {
    static const int max_number_of_errors = 10;
    if (!progress_info_.errors().empty())
    {
      ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
      if (ImGui::TreeNode("Errors (see logs for details)"))
      {
        int cpt = 0;
        for (const auto& error : progress_info_.errors())
        {
          ImGui::TextWrapped("%s", error.c_str());
          ++cpt;
          if (cpt == max_number_of_errors)
          {
            ImGui::TextWrapped("... and %d more errors.", progress_info_.errors().size() - cpt);
            break;
          }
        }
        ImGui::TreePop();
      }
      ImGui::PopStyleColor();
    }
  }

  void InfoWidget::onSequenceUpdated()
  {
    refresh_needed_ = true;
  }

  std::string InfoWidget::formattedTime(const std::chrono::steady_clock::duration& duration) const
  {
    std::ostringstream os;
    auto ns = duration;
    auto h = std::chrono::duration_cast<std::chrono::hours>(ns);
    ns -= h;
    auto m = std::chrono::duration_cast<std::chrono::minutes>(ns);
    ns -= m;
    auto s = std::chrono::duration_cast<std::chrono::seconds>(ns);
    os << std::setfill('0') << std::setw(2) << h.count() << "h:"
      << std::setw(2) << m.count() << "m:"
      << std::setw(2) << s.count() << 's';
    return os.str();
  }

}

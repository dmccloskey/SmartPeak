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
// $Maintainer: Douglas McCloskey $
// $Authors: Douglas McCloskey, Bertrand Boudaud $
// --------------------------------------------------------------------------

#pragma once

#include <SmartPeak/PresetWorkflows/PresetWorkflow.h>

namespace SmartPeak
{
  struct LCMS_MRM_Standards : public PresetWorkflow {

    virtual std::string getName() const override 
    { 
      return "LCMS MRM Standards";
    };

    virtual std::vector<std::string> getWorkflowSteps() const override
    {
      return
      {
        "LOAD_RAW_DATA",
        "MAP_CHROMATOGRAMS",
        "PICK_MRM_FEATURES",
        "CHECK_FEATURES",
        "SELECT_FEATURES",
        "OPTIMIZE_CALIBRATION",
        "STORE_QUANTITATION_METHODS",
        "QUANTIFY_FEATURES",
        "STORE_FEATURES"
      };
    };

    virtual std::string getDescription() const override
    {
      return
        "The workflow process's single reaction monitoring (SRM) data generated by liquid chromatography mass spectrometry instrumentation.\n\n"
        "The workflow first picks, checks, and selects features found in the SRM data for each sample, second optimizes the calibration for each quantitation method for each transition, and third quantifies the features found in the SRM data for each sample.\n\n"
        "The user is responsible for supplying the sequence, transitions, quantitation methods, standards concentrations, parameters, and raw data files. Optional input includes feature filter and QC files. Note that the quantitation method file needs to only specify the internal standard and statistical model for each transition: the quantitation method parameters and statistics will be calculated using the information provided in the standards concentrations.\n\n"
        "The workflow generates individual feature files for each sample in the sequence and a summary of the optimized quantitation methods for each transition in each sequence segment. The user has the option of generating reports that summarize the workflow results after the workflow has run.";
    };

    virtual std::map<std::string, std::vector<std::tuple<std::shared_ptr<Widget>, bool>>> getLayout(const AllWindows& all_windows) const override
    {
      return
      {
        {
          "top",
          {
            {all_windows.statistics_, true},
            {all_windows.workflow_, true},
            {all_windows.chromatogram_plot_widget_, true},
            {all_windows.calibrators_line_plot_, true}
          },
        },
        {
          "bottom",
          {
            {all_windows.quickInfoText_, true}
          },
        },
        {
          "left",
          {
            {all_windows.transitions_explorer_window_, true},
            {all_windows.injections_explorer_window_, true}
          },
        }
      };
    };

  };
}

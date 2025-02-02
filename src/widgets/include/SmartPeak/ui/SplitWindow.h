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
#pragma once

#include <SmartPeak/ui/Widget.h>
#include <SmartPeak/ui/LayoutLoader.h>
#include <SmartPeak/ui/WindowSizesAndPositions.h>
#include <SmartPeak/PresetWorkflows/AllWindows.h>

#include <string>
#include <vector>

namespace SmartPeak
{
  /**
  * @brief The SplitWindow holds 4 sub windows: left, right, top and bottom.
  */
  class SplitWindow final : public Widget
  {
  public:
    SplitWindow(const AllWindows& all_windows) :
      Widget("SplitWindow"),
      all_windows_(all_windows)
    { };

    void draw() override;

    /**
    * @brief Fills the layoutLoader with the sub windows of the SplitWindow
    */
    void setupLayoutLoader(LayoutLoader& layout_loader);

    WindowSizesAndPositions win_size_and_pos_;

    std::map<std::string, std::vector<std::tuple<std::shared_ptr<Widget>,bool>>> default_layout_;

    void resetLayout(const std::map<std::string, std::vector<std::tuple<std::shared_ptr<Widget>, bool>>>& layout);

    const AllWindows& all_windows_;

  protected:
    void showWindows(const std::vector<std::tuple<std::shared_ptr<Widget>, bool>>&windows);
    void focusFirstWindow(const std::vector<std::tuple<std::shared_ptr<Widget>, bool>>& windows);
    bool reset_layout_ = true;
    std::map<std::string, std::vector<std::tuple<std::shared_ptr<Widget>, bool>>> current_layout_;
    std::map<std::string, std::vector<std::tuple<std::shared_ptr<Widget>, bool>>> new_layout_;
  };
}

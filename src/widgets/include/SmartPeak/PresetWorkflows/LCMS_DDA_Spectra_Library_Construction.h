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
  struct LCMS_DDA_Spectra_Library_Construction : public PresetWorkflow {

    virtual std::string getName() const override 
    { 
      return "LCMS DDA Spectra Library Construction";
    };

    virtual std::vector<std::string> getWorkflowSteps() const override
    {
      return
      {
        "LOAD_RAW_DATA",
        "PICK_3D_FEATURES",
        "SEARCH_SPECTRUM_MS1",
        "MERGE_FEATURES_MS1",
        "EXTRACT_SPECTRA_NON_TARGETED",
        "STORE_MSP",
        "STORE_FEATURES"
      };
    };

    virtual std::string getDescription() const override
    {
      return
        "The workflow process's data dependent acquisition (DDA) data generated by liquid chromatography mass spectrometry instrumentation.\n\n"
        "The workflow first picks, annotates, and merges features found in the MS1 scans for each sample, second picks features found in the MS2 scans for each sample, and third constructs a spectral library for downstream spectral annotation.\n\n"
        "The user is responsible for supplying the sequence, accurate mass search, parameters, and raw data files.\n\n"
        "The workflow generates individual feature files and spectral library for each sample in the sequence.";
    };

    virtual std::map<std::string, std::vector<std::tuple<std::shared_ptr<Widget>, bool>>> getLayout(const AllWindows& all_windows) const override
    {
      return
      {
        {
          "top",
          {
            {all_windows.workflow_, true},
            {all_windows.chromatogram_tic_plot_widget_, true}
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
            {all_windows.injections_explorer_window_, true}
          },
        }
      };
    };

  };
}

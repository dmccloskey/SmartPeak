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

#include <SmartPeak/core/FeatureFiltersUtils.h>
#include <SmartPeak/core/ApplicationHandler.h>
#include <SmartPeak/core/FeatureFiltersUtilsMode.h>
#include <SmartPeak/io/InputDataValidation.h>

#include <algorithm>
#include <iostream>
#include <numeric>

#ifndef CSV_IO_NO_THREAD
#define CSV_IO_NO_THREAD
#endif
#include <SmartPeak/io/csv.h>
#include <plog/Log.h>

namespace SmartPeak
{

  bool FeatureFiltersUtils::onFilePicked(const std::filesystem::path& filename,
                                         ApplicationHandler* application_handler,
                                         Filenames& filenames,
                                         const std::string& file_id,
                                         const std::string& file_group_id,
                                         int feature_filter_mode)
  {
    if (application_handler->sequenceHandler_.getSequence().size() == 0)
    {
      LOGE << "File cannot be loaded without first loading the sequence.";
      return false;
    }
    if (feature_filter_mode & FeatureFiltersUtilsMode::EFeatureFiltersModeGroup)
    {
      filenames.setFullPath(file_id, "");
      filenames.setFullPath(file_group_id, filename);
    }
    else if (feature_filter_mode & FeatureFiltersUtilsMode::EFeatureFiltersModeComponent)
    {
      filenames.setFullPath(file_id, filename);
      filenames.setFullPath(file_group_id, "");
    }
    return true;
  }

  void FeatureFiltersUtils::storeFeatureFilters(const std::string& file_id,
                                                const std::string& file_group_id,
                                                Filenames& filenames,
                                                const OpenMS::MRMFeatureQC& features_qc,
                                                int feature_filter_mode)
  {
    if (!InputDataValidation::prepareToStoreOneOfTwo(filenames, file_id, file_group_id))
    {
      return;
    }

    try {
      OpenMS::MRMFeatureQCFile featureQCFile;
      if ((feature_filter_mode & FeatureFiltersUtilsMode::EFeatureFiltersModeComponent)
        && (!filenames.getFullPath(file_id).empty()))  // because we don't know if either of the two names is empty
      {
        if (filenames.isEmbedded(file_id))
        {
          std::vector<int> inserted_rows;
          auto db_context = filenames.getSessionDB().beginWrite(
            file_id,
            "component_name", "TEXT",
            "intensity_l", "REAL",
            "intensity_u", "REAL",
            "overall_quality_l", "REAL",
            "overall_quality_u", "REAL",
            "retention_time_l", "REAL",
            "retention_time_u", "REAL"
          );
          if (!db_context)
          {
            return;
          }
          for (const auto& feature_filter : features_qc.component_qcs)
          {
            filenames.getSessionDB().write(
              *db_context,
              feature_filter.component_name,
              feature_filter.intensity_l,
              feature_filter.intensity_u,
              feature_filter.overall_quality_l,
              feature_filter.overall_quality_u,
              feature_filter.retention_time_l,
              feature_filter.retention_time_u
            );
            inserted_rows.push_back(filenames.getSessionDB().getLastInsertedRowId(*db_context));
          }
          filenames.getSessionDB().endWrite(*db_context);
          std::vector<const std::map<OpenMS::String, std::pair<double, double>>*> metadata_values;
          for (auto& comp : features_qc.component_qcs)
          {
            metadata_values.push_back(&comp.meta_value_qc);
          }
          storeMetadataInDB(filenames, file_id, inserted_rows, metadata_values);
        }
        else
        {
          featureQCFile.store(filenames.getFullPath(file_id).generic_string(), features_qc, false);
        }
      }
      if ((feature_filter_mode & FeatureFiltersUtilsMode::EFeatureFiltersModeGroup) 
          && (!filenames.getFullPath(file_group_id).empty()))
      {
        std::vector<int> inserted_rows;
        if (filenames.isEmbedded(file_group_id))
        {
          auto db_context = filenames.getSessionDB().beginWrite(
            file_group_id,
            "component_group_name", "TEXT",
            "n_heavy_l ", "INT",
            "n_heavy_u", "INT",
            "n_light_l", "INT",
            "n_light_u", "INT",
            "n_detecting_l", "INT",
            "n_detecting_u", "INT",
            "n_quantifying_l", "INT",
            "n_quantifying_u", "INT",
            "n_identifying_l", "INT",
            "n_identifying_u", "INT",
            "n_transitions_l", "INT",
            "n_transitions_u", "INT",
            "ion_ratio_pair_name_1", "STRING",
            "ion_ratio_pair_name_2", "STRING",
            "ion_ratio_l", "REAL",
            "ion_ratio_u", "REAL",
            "retention_time_l", "REAL",
            "retention_time_u", "REAL",
            "intensity_l", "REAL",
            "intensity_u", "REAL",
            "overall_quality_l", "REAL",
            "overall_quality_u", "REAL"
          );
          if (!db_context)
          {
            return;
          }
          for (const auto& feature_filter : features_qc.component_group_qcs)
          {
            filenames.getSessionDB().write(
              *db_context,
              feature_filter.component_group_name,
              feature_filter.n_heavy_l,
              feature_filter.n_heavy_u,
              feature_filter.n_light_l,
              feature_filter.n_light_u,
              feature_filter.n_detecting_l,
              feature_filter.n_detecting_u,
              feature_filter.n_quantifying_l,
              feature_filter.n_quantifying_u,
              feature_filter.n_identifying_l,
              feature_filter.n_identifying_u,
              feature_filter.n_transitions_l,
              feature_filter.n_transitions_u,
              feature_filter.ion_ratio_pair_name_1,
              feature_filter.ion_ratio_pair_name_2,
              feature_filter.ion_ratio_l,
              feature_filter.ion_ratio_u,
              feature_filter.retention_time_l,
              feature_filter.retention_time_u,
              feature_filter.intensity_l,
              feature_filter.intensity_u,
              feature_filter.overall_quality_l,
              feature_filter.overall_quality_u
            );
            inserted_rows.push_back(filenames.getSessionDB().getLastInsertedRowId(*db_context));
          }
          filenames.getSessionDB().endWrite(*db_context);
          std::vector<const std::map<OpenMS::String, std::pair<double, double>>*> metadata_values;
          for (auto& comp : features_qc.component_group_qcs)
          {
            metadata_values.push_back(&comp.meta_value_qc);
          }
          storeMetadataInDB(filenames, file_group_id, inserted_rows, metadata_values);
        }
        else
        {
          featureQCFile.store(filenames.getFullPath(file_group_id).generic_string(), features_qc, true);
        }
      }
    }
    catch (const std::exception& e) {
      LOGE << e.what();
      LOGI << "feature filter store exception";
    }
  }

  void FeatureFiltersUtils::loadFeatureFilters(
    const std::string& file_id,
    const std::string& file_group_id,
    Filenames& filenames,
    OpenMS::MRMFeatureQC& features_qc,
    std::function<void()> notification,
    std::function<void()> notification_group,
    int feature_filter_mode)
  {
    if (!InputDataValidation::prepareToLoadOneOfTwo(filenames, file_id, file_group_id, true))
    {
      return;
    }

    try {
      OpenMS::MRMFeatureQCFile featureQCFile;
      if ((feature_filter_mode & FeatureFiltersUtilsMode::EFeatureFiltersModeComponent)
        && (!filenames.getFullPath(file_id).empty()))  // because we don't know if either of the two names is empty
      {
        if (filenames.isEmbedded(file_id))
        {
          std::vector<int> features_id;
          auto db_context = filenames.getSessionDB().beginRead(
            file_id,
            "ID",
            "component_name",
            "intensity_l",
            "intensity_u",
            "overall_quality_l",
            "overall_quality_u",
            "retention_time_l",
            "retention_time_u"
          );
          if (!db_context)
          {
            return;
          }
          OpenMS::MRMFeatureQC::ComponentQCs feature_filter;
          int feature_id;
          while (filenames.getSessionDB().read(
            *db_context,
            feature_id,
            feature_filter.component_name,
            feature_filter.intensity_l,
            feature_filter.intensity_u,
            feature_filter.overall_quality_l,
            feature_filter.overall_quality_u,
            feature_filter.retention_time_l,
            feature_filter.retention_time_u
          ))
          {
            features_qc.component_qcs.push_back(feature_filter);
            features_id.push_back(feature_id);
          }
          filenames.getSessionDB().endRead(*db_context);
          std::vector<std::map<OpenMS::String, std::pair<double, double>>*> metadata_values;
          for (auto& comp : features_qc.component_qcs)
          {
            metadata_values.push_back(&comp.meta_value_qc);
          }
          loadMetadataFromDB(filenames, file_id, features_id, metadata_values);
        }
        else
        {
          const auto full_path = filenames.getFullPath(file_id);
          if (std::filesystem::exists(full_path))
          {
            OpenMS::MRMFeatureQCFile featureQCFile;
            featureQCFile.load(filenames.getFullPath(file_id).generic_string(), features_qc, false);
            if (notification) notification();
          }
          else
          {
            LOGE << "Failed to load " << full_path.generic_string();
          }
        }
      }
      if ((feature_filter_mode & FeatureFiltersUtilsMode::EFeatureFiltersModeGroup)
        && !filenames.getFullPath(file_group_id).empty())
      {
        if (filenames.isEmbedded(file_group_id))
        {
          std::vector<int> features_id;
          auto db_context = filenames.getSessionDB().beginRead(
            file_group_id,
            "ID",
            "component_group_name",
            "n_heavy_l ",
            "n_heavy_u",
            "n_light_l",
            "n_light_u",
            "n_detecting_l",
            "n_detecting_u",
            "n_quantifying_l",
            "n_quantifying_u",
            "n_identifying_l",
            "n_identifying_u",
            "n_transitions_l",
            "n_transitions_u",
            "ion_ratio_pair_name_1",
            "ion_ratio_pair_name_2",
            "ion_ratio_l",
            "ion_ratio_u",
            "retention_time_l",
            "retention_time_u",
            "intensity_l",
            "intensity_u",
            "overall_quality_l",
            "overall_quality_u"
          );
          if (!db_context)
          {
            return;
          }
          OpenMS::MRMFeatureQC::ComponentGroupQCs feature_filter;
          int feature_id;
          while (filenames.getSessionDB().read(
            *db_context,
            feature_id,
            feature_filter.component_group_name,
            feature_filter.n_heavy_l,
            feature_filter.n_heavy_u,
            feature_filter.n_light_l,
            feature_filter.n_light_u,
            feature_filter.n_detecting_l,
            feature_filter.n_detecting_u,
            feature_filter.n_quantifying_l,
            feature_filter.n_quantifying_u,
            feature_filter.n_identifying_l,
            feature_filter.n_identifying_u,
            feature_filter.n_transitions_l,
            feature_filter.n_transitions_u,
            feature_filter.ion_ratio_pair_name_1,
            feature_filter.ion_ratio_pair_name_2,
            feature_filter.ion_ratio_l,
            feature_filter.ion_ratio_u,
            feature_filter.retention_time_l,
            feature_filter.retention_time_u,
            feature_filter.intensity_l,
            feature_filter.intensity_u,
            feature_filter.overall_quality_l,
            feature_filter.overall_quality_u
          ))
          {
            features_qc.component_group_qcs.push_back(feature_filter);
            features_id.push_back(feature_id);
          }
          filenames.getSessionDB().endRead(*db_context);
          std::vector<std::map<OpenMS::String, std::pair<double, double>>*> metadata_values;
          for (auto& comp : features_qc.component_group_qcs)
          {
            metadata_values.push_back(&comp.meta_value_qc);
          }
          loadMetadataFromDB(filenames, file_group_id, features_id, metadata_values);
        }
        else
        {
          const auto full_path = filenames.getFullPath(file_group_id);
          if (std::filesystem::exists(full_path))
          {
            OpenMS::MRMFeatureQCFile featureQCFile;
            featureQCFile.load(filenames.getFullPath(file_group_id).generic_string(), features_qc, true);
            if (notification_group) notification_group();
          }
          else
          {
            LOGE << "Failed to load " << full_path.generic_string();
          }
        }
      }
    }
    catch (const std::exception& e) {
      features_qc.component_qcs.clear();
      features_qc.component_group_qcs.clear();
      features_qc.component_group_pair_qcs.clear();
      LOGE << e.what();
      LOGI << "feature filter load exception";
    }
  }

  void FeatureFiltersUtils::storeMetadataInDB(
    Filenames& filenames,
    const std::string& file_id,
    const std::vector<int>& inserted_rows,
    const std::vector<const std::map<OpenMS::String, std::pair<double, double>>*> metadata_values)
  {
    auto db_context = filenames.getSessionDB().beginWrite(
      "metadata_" + file_id,
      "feature_id", "INTEGER",
      "name", "TEXT",
      "l", "REAL",
      "u", "REAL"
    );
    if (!db_context)
    {
      return;
    }
    int index = 0;
    for (const auto& feature_filter : metadata_values)
    {
      auto feature_filter_id = inserted_rows.at(index);
      for (const auto metadata : *feature_filter)
      {
        const auto& metadata_name = metadata.first;
        const auto& metadata_value = metadata.second;
        filenames.getSessionDB().write(
          *db_context,
          feature_filter_id,
          metadata_name,
          metadata_value.first,
          metadata_value.second
        );
      }
      ++index;
    }
    filenames.getSessionDB().endWrite(*db_context);
  }

  void FeatureFiltersUtils::loadMetadataFromDB(
    Filenames& filenames,
    const std::string& file_id,
    const std::vector<int>& features_id,
    std::vector<std::map<OpenMS::String, std::pair<double, double>>*> metadata_values)
  {
    int index = 0;
    for (auto& feature_filter : metadata_values)
    {
      auto feature_id = features_id[index];
      auto db_context = filenames.getSessionDB().beginReadWhere(
        "metadata_" + file_id,
        "feature_id",
        feature_id,
        "name",
        "l",
        "u"
      );
      OpenMS::String metadata_name;
      double metadata_value_l;
      double metadata_value_u;
      while (filenames.getSessionDB().read(
        *db_context,
        metadata_name,
        metadata_value_l,
        metadata_value_u
      ))
      {
        feature_filter->emplace(metadata_name, std::make_pair(metadata_value_l, metadata_value_u));
      }
      filenames.getSessionDB().endRead(*db_context);
      ++index;
    }
  }
}


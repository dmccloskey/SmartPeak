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
// $Authors: Douglas McCloskey $
// --------------------------------------------------------------------------

#include <gtest/gtest.h>
#include <SmartPeak/test_config.h>
#include <SmartPeak/core/SampleGroupProcessor.h>
#include <SmartPeak/core/SampleGroupProcessors/MergeInjections.h>
#include <SmartPeak/core/SampleGroupProcessors/LoadFeaturesSampleGroup.h>
#include <SmartPeak/core/SampleGroupProcessors/StoreFeaturesSampleGroup.h>

using namespace SmartPeak;
using namespace std;

void makeSequence(SequenceHandler& sequenceHandler_IO, const bool& test_only_polarity) {
  // ser-L.ser-L_1.Light
  const vector<double> x1 = {
    2.32e4, 2.45e4, 1.78e4, 2.11e4, 1.91e4,
    2.06e4, 1.85e4, 1.53e4
  };
  const vector<double> y1 = {
    4.94e3, 6.55e3, 7.37e3, 1.54e4, 2.87e4,
    5.41e4, 1.16e5, 1.85e5
  };
  const vector<double> z1 = {
    1.00e-2, 2.00e-2, 4.00e-2, 1.00e-1, 2.00e-1,
    4.00e-1, 1.00e0, 2.00e0
  };

  // amp.amp_1.Light
  const vector<double> x2 = {
    2.15e5, 2.32e5, 2.69e5, 2.53e5, 2.50e5,
    2.75e5, 2.67e5, 3.31e5
  };
  const vector<double> y2 = {
      4.40e2, 1.15e3, 1.53e3, 2.01e3, 4.47e3,
      7.36e3, 2.18e4, 4.46e4
  };
  const vector<double> z2 = {
      2.00e-3, 4.00e-3, 8.00e-3, 2.00e-2, 4.00e-2,
      8.00e-2, 2.00e-1, 4.00e-1
  };

  // atp.atp_1.Light
  const vector<double> x3 = {
      8.28e2, 1.32e3, 1.57e3, 1.63e3, 1.48e3,
      2.43e3, 4.44e3, 1.03e4
  };
  const vector<double> y3 = {
      2.21e2, 4.41e2, 3.31e2, 2.21e2, 3.09e2,
      5.96e2, 1.26e3, 2.49e3
  };
  const vector<double> z3 = {
      2.00e-3, 4.00e-3, 8.00e-3, 2.00e-2, 4.00e-2,
      8.00e-2, 2.00e-1, 4.00e-1
  };

  size_t max_iters = x1.size();
  if (test_only_polarity) max_iters = 2;
  for (size_t i = 0; i < max_iters; ++i) {
    const string sample_name = "level" + std::to_string(i);
    OpenMS::FeatureMap feature_map;

    // ser-L.ser-L_1.Light
    OpenMS::Feature mrm_feature;
    OpenMS::Feature component, IS_component;
    component.setMetaValue("native_id", "ser-L.ser-L_1.Light");
    component.setMetaValue("peak_apex_int", x1[i]);
    component.setMetaValue("QC_transition_score", y1[i]);
    component.setMetaValue("calculated_concentration", z1[i]);
    component.setRT(z1[i]);
    component.setMZ(100);
    IS_component.setMetaValue("native_id", "ser-L.ser-L_1.Heavy");
    IS_component.setMetaValue("peak_apex_int", x1[i]);
    IS_component.setMetaValue("QC_transition_score", y1[i]);
    IS_component.setMetaValue("calculated_concentration", z1[i]);
    IS_component.setRT(z1[i]);
    IS_component.setMZ(100);
    mrm_feature.setMetaValue("PeptideRef", "ser-L");
    mrm_feature.setMetaValue("peak_apex_int", x1[i]);
    mrm_feature.setMetaValue("QC_transition_score", y1[i]);
    mrm_feature.setMetaValue("calculated_concentration", z1[i]);
    mrm_feature.setRT(z1[i]);
    mrm_feature.setMZ(100);
    mrm_feature.setSubordinates({ component, IS_component });
    feature_map.push_back(mrm_feature);

    // amp.amp_1.Light
    component.setMetaValue("native_id", "amp.amp_1.Light");
    component.setMetaValue("peak_apex_int", x2[i]);
    component.setMetaValue("QC_transition_score", y2[i]);
    component.setMetaValue("calculated_concentration", z2[i]);
    component.setRT(z2[i]);
    component.setMZ(200);
    IS_component.setMetaValue("native_id", "amp.amp_1.Heavy");
    IS_component.setMetaValue("peak_apex_int", x2[i]);
    IS_component.setMetaValue("QC_transition_score", y2[i]);
    IS_component.setMetaValue("calculated_concentration", z2[i]);
    IS_component.setRT(z2[i]);
    IS_component.setMZ(200);
    mrm_feature.setMetaValue("PeptideRef", "amp");
    mrm_feature.setMetaValue("peak_apex_int", x2[i]);
    mrm_feature.setMetaValue("QC_transition_score", y2[i]);
    mrm_feature.setMetaValue("calculated_concentration", z2[i]);
    mrm_feature.setRT(z2[i]);
    mrm_feature.setMZ(200);
    mrm_feature.setSubordinates({ component, IS_component });
    feature_map.push_back(mrm_feature);

    // atp.atp_1.Light
    component.setMetaValue("native_id", "atp.atp_1.Light");
    component.setMetaValue("peak_apex_int", x3[i]);
    component.setMetaValue("QC_transition_score", y3[i]);
    component.setMetaValue("calculated_concentration", z3[i]);
    component.setRT(z3[i]);
    component.setMZ(300);
    IS_component.setMetaValue("native_id", "atp.atp_1.Heavy");
    IS_component.setMetaValue("peak_apex_int", x3[i]);
    IS_component.setMetaValue("QC_transition_score", y3[i]);
    IS_component.setMetaValue("calculated_concentration", z3[i]);
    IS_component.setRT(z3[i]);
    IS_component.setMZ(300);
    mrm_feature.setMetaValue("PeptideRef", "atp");
    mrm_feature.setMetaValue("peak_apex_int", x3[i]);
    mrm_feature.setMetaValue("QC_transition_score", y3[i]);
    mrm_feature.setMetaValue("calculated_concentration", z3[i]);
    mrm_feature.setRT(z3[i]);
    mrm_feature.setMZ(300);
    mrm_feature.setSubordinates({ component, IS_component });
    feature_map.push_back(mrm_feature);

    feature_map.setPrimaryMSRunPath({ sample_name });

    MetaDataHandler meta_data;
    meta_data.setSampleName(sample_name);
    meta_data.setSampleGroupName("group1");
    meta_data.setSampleType(SampleType::Standard);
    meta_data.setFilename("filename" + std::to_string(i));
    meta_data.setSequenceSegmentName("segment1");
    meta_data.acq_method_name = "6";
    meta_data.inj_volume = 7.0;
    meta_data.inj_volume_units = "8";
    meta_data.batch_name = "9";
    if (test_only_polarity) {
      meta_data.scan_polarity = (i % 2 == 0) ? "positive" : "negative";
      meta_data.scan_mass_low = 100;
      meta_data.scan_mass_high = 1000;
      meta_data.dilution_factor = 1.0;
    }
    else {
      meta_data.scan_polarity = (i < 4) ? "positive" : "negative";
      std::vector<int> ranges = { 0,0,1,1,0,0,1,1 };
      meta_data.scan_mass_low = (ranges.at(i) == 0) ? 50 : 200;
      meta_data.scan_mass_high = (ranges.at(i) == 0) ? 200 : 2000;
      meta_data.dilution_factor = (i % 2 == 0) ? 1.0 : 10.0;
    }

    sequenceHandler_IO.addSampleToSequence(meta_data, OpenMS::FeatureMap());

    RawDataHandler rawDataHandler;
    rawDataHandler.setFeatureMap(feature_map);
    sequenceHandler_IO.getSequence().at(i).setRawData(rawDataHandler);
  }
}

void makeSequenceSameScanMassAndDilution(SequenceHandler& sequenceHandler_IO) {
  // ser-L.ser-L_1.Light
  const vector<double> x1 = {
    1, 2, 3
  };
  const vector<double> y1 = {
    10, 20 ,30
  };
  const vector<double> z1 = {
    100, 200 ,300
  };

  size_t max_iters = x1.size();
  for (size_t i = 0; i < max_iters; ++i) {
    const string sample_name = "level" + std::to_string(i);
    OpenMS::FeatureMap feature_map;

    // ser-L.ser-L_1.Light
    OpenMS::Feature mrm_feature;
    OpenMS::Feature component, IS_component;
    component.setMetaValue("native_id", "ser-L.ser-L_1.Light");
    component.setMetaValue("peak_apex_int", x1[i]);
    component.setMetaValue("QC_transition_score", y1[i]);
    component.setMetaValue("calculated_concentration", z1[i]);
    component.setRT(z1[i]);
    component.setMZ(100);
    IS_component.setMetaValue("native_id", "ser-L.ser-L_1.Heavy");
    IS_component.setMetaValue("peak_apex_int", x1[i]);
    IS_component.setMetaValue("QC_transition_score", y1[i]);
    IS_component.setMetaValue("calculated_concentration", z1[i]);
    IS_component.setRT(z1[i]);
    IS_component.setMZ(100);
    mrm_feature.setMetaValue("PeptideRef", "ser-L");
    mrm_feature.setMetaValue("peak_apex_int", x1[i]);
    mrm_feature.setMetaValue("QC_transition_score", y1[i]);
    mrm_feature.setMetaValue("calculated_concentration", z1[i]);
    mrm_feature.setRT(z1[i]);
    mrm_feature.setMZ(100);
    mrm_feature.setSubordinates({ component, IS_component });
    feature_map.push_back(mrm_feature);

    feature_map.setPrimaryMSRunPath({ sample_name });

    MetaDataHandler meta_data;
    meta_data.setSampleName(sample_name);
    meta_data.setSampleGroupName("group1");
    meta_data.setSampleType(SampleType::Standard);
    meta_data.setFilename("filename" + std::to_string(i));
    meta_data.setSequenceSegmentName("segment1");
    meta_data.acq_method_name = "6";
    meta_data.inj_volume = 7.0;
    meta_data.inj_volume_units = "8";
    meta_data.batch_name = "9";

    meta_data.scan_polarity = "negative";
    meta_data.scan_mass_low = -1;
    meta_data.scan_mass_high = -1;
    meta_data.dilution_factor = 1;

    sequenceHandler_IO.addSampleToSequence(meta_data, OpenMS::FeatureMap());

    RawDataHandler rawDataHandler;
    rawDataHandler.setFeatureMap(feature_map);
    sequenceHandler_IO.getSequence().at(i).setRawData(rawDataHandler);
  }
}

/**
  MergeInjections Tests
*/
TEST(MergeInjections, constructorMergeInjections)
{
  MergeInjections* ptrMergeInjections = nullptr;
  MergeInjections* nullPointerMergeInjections = nullptr;
  EXPECT_EQ(ptrMergeInjections, nullPointerMergeInjections);
}

TEST(MergeInjections, destructorMergeInjections)
{
  MergeInjections* ptrMergeInjections = nullptr;
  ptrMergeInjections = new MergeInjections();
  delete ptrMergeInjections;
}

TEST(MergeInjections, gettersMergeInjections)
{
  MergeInjections processor;
  EXPECT_STREQ(processor.getName().c_str(), "MERGE_INJECTIONS");
}

TEST(SampleGroupHandler, processMergeInjections)
{
  // setup the parameters
  ParameterSet mergeinjs_params({{"MergeInjections", {
    {
      {"name", "scan_polarity_merge_rule"},
      {"value", "WeightedMean"}
    },
    {
      {"name", "mass_range_merge_rule"},
      {"value", "Sum"}
    },
    {
      {"name", "dilution_series_merge_rule"},
      {"value", "Max"}
    },
    {
      {"name", "scan_polarity_merge_feature_name"},
      {"value", "calculated_concentration"}
    },
    {
      {"name", "mass_range_merge_feature_name"},
      {"value", "peak_apex_int"}
    },
    {
      {"name", "dilution_series_merge_feature_name"},
      {"value", "QC_transition_score"}
    },
    {
      {"name", "merge_subordinates"},
      {"value", "true"}
    }
  }}});

  // setup the sequence
  SequenceHandler sequenceHandler;
  makeSequence(sequenceHandler, false);
  SampleGroupHandler sampleGroupHandler = sequenceHandler.getSampleGroups().front();

  // test merge injections on all with subordinates
  MergeInjections sampleGroupProcessor;
  Filenames filenames;
  sampleGroupProcessor.process(sampleGroupHandler, sequenceHandler, mergeinjs_params, filenames);

  EXPECT_EQ(sampleGroupHandler.getFeatureMap().size(), 3);
  EXPECT_EQ(sampleGroupHandler.getFeatureMap().at(0).getSubordinates().size(), 2);
  EXPECT_STREQ(sampleGroupHandler.getFeatureMap().at(0).getMetaValue("PeptideRef").toString().c_str(), "amp");
  EXPECT_STREQ(sampleGroupHandler.getFeatureMap().at(0).getSubordinates().at(0).getMetaValue("native_id").toString().c_str(), "amp.amp_1.Heavy");
  EXPECT_NEAR(static_cast<float>(sampleGroupHandler.getFeatureMap().at(0).getSubordinates().at(0).getMetaValue("peak_apex_int")), 600238.125, 1e-4);
  EXPECT_NEAR(static_cast<float>(sampleGroupHandler.getFeatureMap().at(0).getSubordinates().at(0).getMetaValue("QC_transition_score")), 49636.1914, 1e-4);
  EXPECT_NEAR(static_cast<float>(sampleGroupHandler.getFeatureMap().at(0).getSubordinates().at(0).getMetaValue("calculated_concentration")), 0.458285719, 1e-4);
  EXPECT_NEAR(static_cast<float>(sampleGroupHandler.getFeatureMap().at(0).getSubordinates().at(0).getRT()), 0.458285719, 1e-4);
  EXPECT_NEAR(static_cast<float>(sampleGroupHandler.getFeatureMap().at(0).getSubordinates().at(0).getMZ()), 400, 1e-4);

  // test merge injections on all with out subordinates
  mergeinjs_params.at("MergeInjections").at(6).setValueFromString("false");
  sampleGroupProcessor.process(sampleGroupHandler, sequenceHandler, mergeinjs_params, filenames);

  EXPECT_EQ(sampleGroupHandler.getFeatureMap().size(), 3);
  EXPECT_EQ(sampleGroupHandler.getFeatureMap().at(0).getSubordinates().size(), 1);
  const OpenMS::Feature& feat1 = sampleGroupHandler.getFeatureMap().at(0);
  EXPECT_STREQ(feat1.getMetaValue("PeptideRef").toString().c_str(), "amp");
  EXPECT_NEAR(static_cast<float>(feat1.getMetaValue("peak_apex_int")), 600238.125, 1e-4);
  EXPECT_NEAR(static_cast<float>(feat1.getMetaValue("QC_transition_score")), 49636.1914, 1e-4);
  EXPECT_NEAR(static_cast<float>(feat1.getMetaValue("calculated_concentration")), 0.458285719, 1e-4);
  EXPECT_NEAR(static_cast<float>(feat1.getRT()), 0.458285719, 1e-4);
  EXPECT_NEAR(static_cast<float>(feat1.getMZ()), 400, 1e-4);
  const OpenMS::Feature& sub1 = sampleGroupHandler.getFeatureMap().at(0).getSubordinates().at(0);
  EXPECT_STREQ(sub1.getMetaValue("native_id").toString().c_str(), "amp");
  EXPECT_NEAR(static_cast<float>(sub1.getMetaValue("peak_apex_int")), 600238.125, 1e-4);
  EXPECT_NEAR(static_cast<float>(sub1.getMetaValue("QC_transition_score")), 49636.1914, 1e-4);
  EXPECT_NEAR(static_cast<float>(sub1.getMetaValue("calculated_concentration")), 0.458285719, 1e-4);
  EXPECT_NEAR(static_cast<float>(sub1.getRT()), 0.458285719, 1e-4);
  EXPECT_NEAR(static_cast<float>(sub1.getMZ()), 400, 1e-4);

  // Test merge injections on polarities with Max
  sequenceHandler.clear();
  makeSequence(sequenceHandler, true);
  sampleGroupHandler = sequenceHandler.getSampleGroups().front();
  mergeinjs_params.at("MergeInjections").at(0).setValueFromString("Max");
  mergeinjs_params.at("MergeInjections").at(6).setValueFromString("true");
  sampleGroupProcessor.process(sampleGroupHandler, sequenceHandler, mergeinjs_params, filenames);

  EXPECT_EQ(sampleGroupHandler.getFeatureMap().size(), 3);
  EXPECT_EQ(sampleGroupHandler.getFeatureMap().at(0).getSubordinates().size(), 2);
  EXPECT_STREQ(sampleGroupHandler.getFeatureMap().at(0).getMetaValue("PeptideRef").toString().c_str(), "amp");
  EXPECT_STREQ(sampleGroupHandler.getFeatureMap().at(0).getSubordinates().at(0).getMetaValue("native_id").toString().c_str(), "amp.amp_1.Heavy");
  EXPECT_NEAR(static_cast<float>(sampleGroupHandler.getFeatureMap().at(0).getSubordinates().at(0).getMetaValue("peak_apex_int")), 232000, 1e-4);
  EXPECT_NEAR(static_cast<float>(sampleGroupHandler.getFeatureMap().at(0).getSubordinates().at(0).getMetaValue("QC_transition_score")), 1150, 1e-4);
  EXPECT_NEAR(static_cast<float>(sampleGroupHandler.getFeatureMap().at(0).getSubordinates().at(0).getMetaValue("calculated_concentration")), 0.004, 1e-4);
  EXPECT_NEAR(static_cast<float>(sampleGroupHandler.getFeatureMap().at(0).getSubordinates().at(0).getRT()), 0.004, 1e-4);
  EXPECT_NEAR(static_cast<float>(sampleGroupHandler.getFeatureMap().at(0).getSubordinates().at(0).getMZ()), 200, 1e-4);

  // Test merge injections on polarities with Min
  sequenceHandler.clear();
  makeSequence(sequenceHandler, true);
  sampleGroupHandler = sequenceHandler.getSampleGroups().front();
  mergeinjs_params.at("MergeInjections").at(0).setValueFromString("Min");
  sampleGroupProcessor.process(sampleGroupHandler, sequenceHandler, mergeinjs_params, filenames);

  EXPECT_EQ(sampleGroupHandler.getFeatureMap().size(), 3);
  EXPECT_EQ(sampleGroupHandler.getFeatureMap().at(0).getSubordinates().size(), 2);
  EXPECT_STREQ(sampleGroupHandler.getFeatureMap().at(0).getMetaValue("PeptideRef").toString().c_str(), "amp");
  EXPECT_STREQ(sampleGroupHandler.getFeatureMap().at(0).getSubordinates().at(0).getMetaValue("native_id").toString().c_str(), "amp.amp_1.Heavy");
  EXPECT_NEAR(static_cast<float>(sampleGroupHandler.getFeatureMap().at(0).getSubordinates().at(0).getMetaValue("peak_apex_int")), 215000, 1e-4);
  EXPECT_NEAR(static_cast<float>(sampleGroupHandler.getFeatureMap().at(0).getSubordinates().at(0).getMetaValue("QC_transition_score")), 440, 1e-4);
  EXPECT_NEAR(static_cast<float>(sampleGroupHandler.getFeatureMap().at(0).getSubordinates().at(0).getMetaValue("calculated_concentration")), 0.002, 1e-4);
  EXPECT_NEAR(static_cast<float>(sampleGroupHandler.getFeatureMap().at(0).getSubordinates().at(0).getRT()), 0.002, 1e-4);
  EXPECT_NEAR(static_cast<float>(sampleGroupHandler.getFeatureMap().at(0).getSubordinates().at(0).getMZ()), 200, 1e-4);

  // Test merge injections on polarities with Sum
  sequenceHandler.clear();
  makeSequence(sequenceHandler, true);
  sampleGroupHandler = sequenceHandler.getSampleGroups().front();
  mergeinjs_params.at("MergeInjections").at(0).setValueFromString("Sum");
  sampleGroupProcessor.process(sampleGroupHandler, sequenceHandler, mergeinjs_params, filenames);

  EXPECT_EQ(sampleGroupHandler.getFeatureMap().size(), 3);
  EXPECT_EQ(sampleGroupHandler.getFeatureMap().at(0).getSubordinates().size(), 2);
  EXPECT_STREQ(sampleGroupHandler.getFeatureMap().at(0).getMetaValue("PeptideRef").toString().c_str(), "amp");
  EXPECT_STREQ(sampleGroupHandler.getFeatureMap().at(0).getSubordinates().at(0).getMetaValue("native_id").toString().c_str(), "amp.amp_1.Heavy");
  EXPECT_NEAR(static_cast<float>(sampleGroupHandler.getFeatureMap().at(0).getSubordinates().at(0).getMetaValue("peak_apex_int")), 4.47E+05, 1e-4);
  EXPECT_NEAR(static_cast<float>(sampleGroupHandler.getFeatureMap().at(0).getSubordinates().at(0).getMetaValue("QC_transition_score")), 1.59E+03, 1e-4);
  EXPECT_NEAR(static_cast<float>(sampleGroupHandler.getFeatureMap().at(0).getSubordinates().at(0).getMetaValue("calculated_concentration")), 6.00E-03, 1e-4);
  EXPECT_NEAR(static_cast<float>(sampleGroupHandler.getFeatureMap().at(0).getSubordinates().at(0).getRT()), 6.00E-03, 1e-4);
  EXPECT_NEAR(static_cast<float>(sampleGroupHandler.getFeatureMap().at(0).getSubordinates().at(0).getMZ()), 400, 1e-4);

  // Test merge injections on polarities with Mean
  sequenceHandler.clear();
  makeSequence(sequenceHandler, true);
  sampleGroupHandler = sequenceHandler.getSampleGroups().front();
  mergeinjs_params.at("MergeInjections").at(0).setValueFromString("Mean");
  sampleGroupProcessor.process(sampleGroupHandler, sequenceHandler, mergeinjs_params, filenames);

  EXPECT_EQ(sampleGroupHandler.getFeatureMap().size(), 3);
  EXPECT_EQ(sampleGroupHandler.getFeatureMap().at(0).getSubordinates().size(), 2);
  EXPECT_STREQ(sampleGroupHandler.getFeatureMap().at(0).getMetaValue("PeptideRef").toString().c_str(), "amp");
  EXPECT_STREQ(sampleGroupHandler.getFeatureMap().at(0).getSubordinates().at(0).getMetaValue("native_id").toString().c_str(), "amp.amp_1.Heavy");
  EXPECT_NEAR(static_cast<float>(sampleGroupHandler.getFeatureMap().at(0).getSubordinates().at(0).getMetaValue("peak_apex_int")), 223500, 1e-4);
  EXPECT_NEAR(static_cast<float>(sampleGroupHandler.getFeatureMap().at(0).getSubordinates().at(0).getMetaValue("QC_transition_score")), 7.95E+02, 1e-4);
  EXPECT_NEAR(static_cast<float>(sampleGroupHandler.getFeatureMap().at(0).getSubordinates().at(0).getMetaValue("calculated_concentration")), 3.00E-03, 1e-4);
  EXPECT_NEAR(static_cast<float>(sampleGroupHandler.getFeatureMap().at(0).getSubordinates().at(0).getRT()), 3.00E-03, 1e-4);
  EXPECT_NEAR(static_cast<float>(sampleGroupHandler.getFeatureMap().at(0).getSubordinates().at(0).getMZ()), 200, 1e-4);

  // Test merge injections on polarities with WeightedMean
  sequenceHandler.clear();
  makeSequence(sequenceHandler, true);
  sampleGroupHandler = sequenceHandler.getSampleGroups().front();
  mergeinjs_params.at("MergeInjections").at(0).setValueFromString("WeightedMean");
  sampleGroupProcessor.process(sampleGroupHandler, sequenceHandler, mergeinjs_params, filenames);

  EXPECT_EQ(sampleGroupHandler.getFeatureMap().size(), 3);
  EXPECT_EQ(sampleGroupHandler.getFeatureMap().at(0).getSubordinates().size(), 2);
  EXPECT_STREQ(sampleGroupHandler.getFeatureMap().at(0).getMetaValue("PeptideRef").toString().c_str(), "amp");
  EXPECT_STREQ(sampleGroupHandler.getFeatureMap().at(0).getSubordinates().at(0).getMetaValue("native_id").toString().c_str(), "amp.amp_1.Heavy");
  EXPECT_NEAR(static_cast<float>(sampleGroupHandler.getFeatureMap().at(0).getSubordinates().at(0).getMetaValue("peak_apex_int")), 226333.344, 1e-3);
  EXPECT_NEAR(static_cast<float>(sampleGroupHandler.getFeatureMap().at(0).getSubordinates().at(0).getMetaValue("QC_transition_score")), 913.333374, 1e-4);
  EXPECT_NEAR(static_cast<float>(sampleGroupHandler.getFeatureMap().at(0).getSubordinates().at(0).getMetaValue("calculated_concentration")), 0.00333333365, 1e-4);
  EXPECT_NEAR(static_cast<float>(sampleGroupHandler.getFeatureMap().at(0).getSubordinates().at(0).getRT()), 0.00333333365, 1e-4);
  EXPECT_NEAR(static_cast<float>(sampleGroupHandler.getFeatureMap().at(0).getSubordinates().at(0).getMZ()), 200, 1e-4);
}

TEST(SampleGroupHandler, processMergeInjections2)
{
  // setup the parameters
  ParameterSet mergeinjs_params({ {"MergeInjections", {
    {
      {"name", "scan_polarity_merge_rule"},
      {"value", "WeightedMean"}
    },
    {
      {"name", "mass_range_merge_rule"},
      {"value", "Sum"}
    },
    {
      {"name", "dilution_series_merge_rule"},
      {"value", "Max"}
    },
    {
      {"name", "scan_polarity_merge_feature_name"},
      {"value", "peak_apex_int"}
    },
    {
      {"name", "mass_range_merge_feature_name"},
      {"value", "peak_apex_int"}
    },
    {
      {"name", "dilution_series_merge_feature_name"},
      {"value", "peak_apex_int"}
    },
    {
      {"name", "merge_subordinates"},
      {"value", "true"}
    }
  }} });

  // setup the sequence
  SequenceHandler sequenceHandler;
  makeSequenceSameScanMassAndDilution(sequenceHandler);
  SampleGroupHandler sampleGroupHandler = sequenceHandler.getSampleGroups().front();

  // test merge injections on all with subordinates
  MergeInjections sampleGroupProcessor;
  Filenames filenames;
  sampleGroupProcessor.process(sampleGroupHandler, sequenceHandler, mergeinjs_params, filenames);

  EXPECT_EQ(sampleGroupHandler.getFeatureMap().size(), 1);
  EXPECT_EQ(sampleGroupHandler.getFeatureMap().at(0).getSubordinates().size(), 2);
  EXPECT_STREQ(sampleGroupHandler.getFeatureMap().at(0).getMetaValue("PeptideRef").toString().c_str(), "ser-L");
  EXPECT_STREQ(sampleGroupHandler.getFeatureMap().at(0).getSubordinates().at(0).getMetaValue("native_id").toString().c_str(), "ser-L.ser-L_1.Heavy");
  EXPECT_NEAR(static_cast<float>(sampleGroupHandler.getFeatureMap().at(0).getSubordinates().at(0).getMetaValue("peak_apex_int")), 3, 1e-4);
  EXPECT_NEAR(static_cast<float>(sampleGroupHandler.getFeatureMap().at(0).getSubordinates().at(0).getMetaValue("QC_transition_score")), 30, 1e-4);
  EXPECT_NEAR(static_cast<float>(sampleGroupHandler.getFeatureMap().at(0).getSubordinates().at(0).getMetaValue("calculated_concentration")), 300, 1e-4);
  EXPECT_NEAR(static_cast<float>(sampleGroupHandler.getFeatureMap().at(0).getSubordinates().at(0).getRT()), 300, 1e-4);
  EXPECT_NEAR(static_cast<float>(sampleGroupHandler.getFeatureMap().at(0).getSubordinates().at(0).getMZ()), 100, 1e-4);
}

TEST(SampleGroupHandler, processMergeInjections_selectDilution)
{
  // setup the parameters
  ParameterSet mergeinjs_params({ {"MergeInjections", {
    {
      {"name", "scan_polarity_merge_rule"},
      {"value", "Max"}
    },
    {
      {"name", "mass_range_merge_rule"},
      {"value", "Max"}
    },
    {
      {"name", "dilution_series_merge_rule"},
      {"value", "Max"}
    },
    {
      {"name", "scan_polarity_merge_feature_name"},
      {"value", "peak_apex_int"}
    },
    {
      {"name", "mass_range_merge_feature_name"},
      {"value", "peak_apex_int"}
    },
    {
      {"name", "dilution_series_merge_feature_name"},
      {"value", "peak_apex_int"}
    },
    {
      {"name", "merge_subordinates"},
      {"value", "true"}
    },
    {
      {"name", "select_preferred_dilutions"},
      {"value", "true"}
    }
    ,
    {
      {"name", "select_preferred_dilutions_file"},
      {"value", SMARTPEAK_GET_TEST_DATA_PATH("SampleGroupProcessor_selectDilutions.csv")} // contains ser-L.ser-L_1.Heavy, 20
    }
  }} });

  // setup the sequence
  SequenceHandler sequenceHandler;
  // ser-L.ser-L_1.Light
  const vector<double> peak_apex_int = {
    1, 2, 3
  };
  const vector<double> transition_score = {
    10, 20 ,30
  };
  const vector<double> calculated_concentration = {
    100, 200 ,300
  };
  const vector<double> dilutions = {
    10, 20 ,1
  };

  size_t max_iters = peak_apex_int.size();
  for (size_t i = 0; i < max_iters; ++i) {
    const string sample_name = "level" + std::to_string(i);
    OpenMS::FeatureMap feature_map;

    // ser-L.ser-L_1.Light
    OpenMS::Feature mrm_feature;
    OpenMS::Feature component, IS_component;
    component.setMetaValue("native_id", "ser-L.ser-L_1.Light");
    component.setMetaValue("peak_apex_int", peak_apex_int[i]);
    component.setMetaValue("QC_transition_score", transition_score[i]);
    component.setMetaValue("calculated_concentration", calculated_concentration[i]);
    component.setRT(calculated_concentration[i]);
    component.setMZ(100);
    IS_component.setMetaValue("native_id", "ser-L.ser-L_1.Heavy");
    IS_component.setMetaValue("peak_apex_int", peak_apex_int[i]);
    IS_component.setMetaValue("QC_transition_score", transition_score[i]);
    IS_component.setMetaValue("calculated_concentration", calculated_concentration[i]);
    IS_component.setRT(calculated_concentration[i]);
    IS_component.setMZ(100);
    mrm_feature.setMetaValue("PeptideRef", "ser-L");
    mrm_feature.setMetaValue("peak_apex_int", peak_apex_int[i]);
    mrm_feature.setMetaValue("QC_transition_score", transition_score[i]);
    mrm_feature.setMetaValue("calculated_concentration", calculated_concentration[i]);
    mrm_feature.setRT(calculated_concentration[i]);
    mrm_feature.setMZ(100);
    mrm_feature.setSubordinates({ component, IS_component });
    feature_map.push_back(mrm_feature);

    feature_map.setPrimaryMSRunPath({ sample_name });

    MetaDataHandler meta_data;
    meta_data.setSampleName(sample_name);
    meta_data.setSampleGroupName("group1");
    meta_data.setSampleType(SampleType::Standard);
    meta_data.setFilename("filename" + std::to_string(i));
    meta_data.setSequenceSegmentName("segment1");
    meta_data.acq_method_name = "6";
    meta_data.inj_volume = 7.0;
    meta_data.inj_volume_units = "8";
    meta_data.batch_name = "9";

    meta_data.scan_polarity = "negative";
    meta_data.scan_mass_low = -1;
    meta_data.scan_mass_high = -1;
    meta_data.dilution_factor = dilutions[i];

    sequenceHandler.addSampleToSequence(meta_data, OpenMS::FeatureMap());

    RawDataHandler rawDataHandler;
    rawDataHandler.setFeatureMap(feature_map);
    sequenceHandler.getSequence().at(i).setRawData(rawDataHandler);
  }
  SampleGroupHandler sampleGroupHandler = sequenceHandler.getSampleGroups().front();

  // test merge injections on all with subordinates
  MergeInjections sampleGroupProcessor;
  Filenames filenames;
  sampleGroupProcessor.process(sampleGroupHandler, sequenceHandler, mergeinjs_params, filenames);

  EXPECT_EQ(sampleGroupHandler.getFeatureMap().size(), 1);
  EXPECT_EQ(sampleGroupHandler.getFeatureMap().at(0).getSubordinates().size(), 2);
  EXPECT_STREQ(sampleGroupHandler.getFeatureMap().at(0).getMetaValue("PeptideRef").toString().c_str(), "ser-L");
  EXPECT_STREQ(sampleGroupHandler.getFeatureMap().at(0).getSubordinates().at(0).getMetaValue("native_id").toString().c_str(), "ser-L.ser-L_1.Heavy");
  EXPECT_NEAR(static_cast<float>(sampleGroupHandler.getFeatureMap().at(0).getSubordinates().at(0).getMetaValue("peak_apex_int")), 2, 1e-4);
  EXPECT_NEAR(static_cast<float>(sampleGroupHandler.getFeatureMap().at(0).getSubordinates().at(0).getMetaValue("QC_transition_score")), 20, 1e-4);
  EXPECT_NEAR(static_cast<float>(sampleGroupHandler.getFeatureMap().at(0).getSubordinates().at(0).getMetaValue("calculated_concentration")), 200, 1e-4);
  EXPECT_NEAR(static_cast<float>(sampleGroupHandler.getFeatureMap().at(0).getSubordinates().at(0).getRT()), 200, 1e-4);
  EXPECT_NEAR(static_cast<float>(sampleGroupHandler.getFeatureMap().at(0).getSubordinates().at(0).getMZ()), 100, 1e-4);
}

/**
  LoadFeaturesSampleGroup Tests
*/
TEST(LoadFeaturesSampleGroup, constructorLoadFeaturesSampleGroup)
{
  LoadFeaturesSampleGroup* ptrLoadFeaturesSampleGroup = nullptr;
  LoadFeaturesSampleGroup* nullPointerLoadFeaturesSampleGroup = nullptr;
  EXPECT_EQ(ptrLoadFeaturesSampleGroup, nullPointerLoadFeaturesSampleGroup);
}

TEST(LoadFeaturesSampleGroup, destructorLoadFeaturesSampleGroup)
{
  LoadFeaturesSampleGroup* ptrLoadFeaturesSampleGroup = nullptr;
  ptrLoadFeaturesSampleGroup = new LoadFeaturesSampleGroup();
  delete ptrLoadFeaturesSampleGroup;
}

TEST(LoadFeaturesSampleGroup, gettersLoadFeaturesSampleGroup)
{
  LoadFeaturesSampleGroup processor;
  EXPECT_EQ(processor.getName(), "LOAD_FEATURES_SAMPLE_GROUP");
}

TEST(SequenceHandler, processLoadFeaturesSampleGroup)
{
  // setup the sequence
  SequenceHandler sequenceHandler;
  makeSequence(sequenceHandler, false);
  SampleGroupHandler sampleGroupHandler = sequenceHandler.getSampleGroups().front();

  // test store features
  LoadFeaturesSampleGroup process;
  Filenames filenames;
  filenames.setFullPath("featureXMLSampleGroup_i", SMARTPEAK_GET_TEST_DATA_PATH(sampleGroupHandler.getSampleGroupName() + ".featureXML"));
  process.process(sampleGroupHandler, sequenceHandler, {}, filenames);

  EXPECT_EQ(sampleGroupHandler.getFeatureMap().size(), 3);
  EXPECT_EQ(sampleGroupHandler.getFeatureMap().at(0).getSubordinates().size(), 2);
  EXPECT_STREQ(sampleGroupHandler.getFeatureMap().at(0).getMetaValue("PeptideRef").toString().c_str(), "amp");
  EXPECT_STREQ(sampleGroupHandler.getFeatureMap().at(0).getSubordinates().at(0).getMetaValue("native_id").toString().c_str(), "amp.amp_1.Heavy");
  EXPECT_NEAR(static_cast<float>(sampleGroupHandler.getFeatureMap().at(0).getSubordinates().at(0).getMetaValue("peak_apex_int")), 600238.125, 1e-4);
  EXPECT_NEAR(static_cast<float>(sampleGroupHandler.getFeatureMap().at(0).getSubordinates().at(0).getMetaValue("QC_transition_score")), 49636.1914, 1e-4);
  EXPECT_NEAR(static_cast<float>(sampleGroupHandler.getFeatureMap().at(0).getSubordinates().at(0).getMetaValue("calculated_concentration")), 0.458285719, 1e-4);
  EXPECT_NEAR(static_cast<float>(sampleGroupHandler.getFeatureMap().at(0).getSubordinates().at(0).getRT()), 0.458285719, 1e-4);
  EXPECT_NEAR(static_cast<float>(sampleGroupHandler.getFeatureMap().at(0).getSubordinates().at(0).getMZ()), 400, 1e-4);
}

/**
  StoreFeaturesSampleGroup Tests
*/
TEST(StoreFeaturesSampleGroup, constructorStoreFeaturesSampleGroup)
{
  StoreFeaturesSampleGroup* ptrStoreFeaturesSampleGroup = nullptr;
  StoreFeaturesSampleGroup* nullPointerStoreFeaturesSampleGroup = nullptr;
  EXPECT_EQ(ptrStoreFeaturesSampleGroup, nullPointerStoreFeaturesSampleGroup);
}

TEST(StoreFeaturesSampleGroup, destructorStoreFeaturesSampleGroup)
{
  StoreFeaturesSampleGroup* ptrStoreFeaturesSampleGroup = nullptr;
  ptrStoreFeaturesSampleGroup = new StoreFeaturesSampleGroup();
  delete ptrStoreFeaturesSampleGroup;
}

TEST(StoreFeaturesSampleGroup, gettersStoreFeaturesSampleGroup)
{
  StoreFeaturesSampleGroup processor;
  EXPECT_EQ(processor.getName(), "STORE_FEATURES_SAMPLE_GROUP");
}

TEST(SequenceHandler, processStoreFeaturesSampleGroup)
{
  // no tests, it wraps OpenMS store function
}

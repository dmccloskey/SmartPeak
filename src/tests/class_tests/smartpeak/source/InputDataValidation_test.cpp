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
// $Maintainer: Douglas McCloskey, Ahmed Khalil $
// $Authors: Douglas McCloskey $
// --------------------------------------------------------------------------

#include <gtest/gtest.h>
#include <SmartPeak/test_config.h>
#include <SmartPeak/io/InputDataValidation.h>
#include <SmartPeak/core/Filenames.h>
#include <SmartPeak/core/SequenceProcessor.h>
#include <SmartPeak/core/Utilities.h>
#include <SmartPeak/core/RawDataProcessors/LoadTransitions.h>
#include <SmartPeak/core/SequenceSegmentProcessors/LoadQuantitationMethods.h>
#include <SmartPeak/core/ApplicationProcessors/LoadSession.h>

#include <plog/Log.h>
#include <plog/Appenders/ConsoleAppender.h>

using namespace SmartPeak;
using namespace std;

const std::string main_dir = SMARTPEAK_GET_TEST_DATA_PATH("workflow_csv_files");

plog::ConsoleAppender<plog::TxtFormatter> consoleAppender;

TEST(InputDataValidation, fileExists)
{
  string pathname = SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_standardsConcentrations_1.csv");
  EXPECT_TRUE(InputDataValidation::fileExists(pathname));
  pathname = SMARTPEAK_GET_TEST_DATA_PATH("this_does_not_exist.csv");
  EXPECT_FALSE(InputDataValidation::fileExists(pathname));
}

TEST(InputDataValidation, isValidFilename)
{
  struct ExistingFile : public IFilenamesHandler
  {
    void getFilenames(Filenames& filenames) const
    {
      filenames.addFileName("some standards file", "${MAIN_DIR}/OpenMSFile_standardsConcentrations_1.csv");
    };
  };
  struct NonExistingFile : public IFilenamesHandler
  {
    void getFilenames(Filenames& filenames) const
    {
      filenames.addFileName("a file that does not exist", "${MAIN_DIR}/this_does_not_exist.csv");
    };
  };
  struct EmptyFileName : public IFilenamesHandler
  {
    void getFilenames(Filenames& filenames) const
    {
      filenames.addFileName("an empty pathname", "");
    };
  };
  Filenames filenames;
  filenames.setFullPath("some standards file", SMARTPEAK_GET_TEST_DATA_PATH("OpenMSFile_standardsConcentrations_1.csv"));
  EXPECT_TRUE(InputDataValidation::precheckProcessorInputs(ExistingFile(), "some standards file", filenames, true));
  filenames.setFullPath("a file that does not exist", SMARTPEAK_GET_TEST_DATA_PATH("this_does_not_exist.csv"));
  EXPECT_FALSE(InputDataValidation::precheckProcessorInputs(NonExistingFile(), "a file that does not exist", filenames, true));
  EXPECT_TRUE(InputDataValidation::precheckProcessorInputs(NonExistingFile(), "a file that does not exist", filenames, false));
  filenames.setFullPath("an empty pathname", "");
  EXPECT_FALSE(InputDataValidation::precheckProcessorInputs(EmptyFileName(), "an empty pathname", filenames, true));
  EXPECT_TRUE(InputDataValidation::precheckProcessorInputs(EmptyFileName(), "an empty pathname", filenames, false));
}

TEST(InputDataValidation, validateNamesInStructures)
{
  const set<string> s1 = {"1", "2", "3"};
  const set<string> s2 = {"1", "2", "4"};
  const set<string> s3 = {"1", "2", "3", "4"};
  EXPECT_TRUE(InputDataValidation::validateNamesInStructures(s1, s1, "", "", true)); // check itself
  EXPECT_FALSE(InputDataValidation::validateNamesInStructures(s1, s2, "", "", false)); // s1 -> s2, "3" is missing
  EXPECT_FALSE(InputDataValidation::validateNamesInStructures(s2, s1, "", "", false)); // s1 -> s2, "4" is missing
  EXPECT_FALSE(InputDataValidation::validateNamesInStructures(s2, s1, "", "", true)); // s1 <-> s2, "3" and "4" are missing
  EXPECT_TRUE(InputDataValidation::validateNamesInStructures(s1, s3, "", "", false)); // s1 -> s3, no name is missing
  EXPECT_FALSE(InputDataValidation::validateNamesInStructures(s1, s3, "", "", true)); // s1 <-> s3, "4" is missing
}

TEST(InputDataValidation, findMissingNames)
{
  const set<string> s1 = {"1", "2", "3"};
  const set<string> s2 = {"1", "2", "4"};
  const set<string> s3 = {"1", "2", "3", "4"};
  set<string> result;

  result = InputDataValidation::findMissingNames(s1, s1);
  EXPECT_EQ(result.size(), 0);

  result = InputDataValidation::findMissingNames(s1, s2);
  EXPECT_EQ(result.size(), 1);
  EXPECT_EQ(result.count("3"), 1);

  result = InputDataValidation::findMissingNames(s2, s1);
  EXPECT_EQ(result.size(), 1);
  EXPECT_EQ(result.count("4"), 1);

  result = InputDataValidation::findMissingNames(s1, s3);
  EXPECT_EQ(result.size(), 0);

  result = InputDataValidation::findMissingNames(s3, s1);
  EXPECT_EQ(result.size(), 1);
  EXPECT_EQ(result.count("4"), 1);
}

TEST(InputDataValidation, sampleNamesAreConsistent)
{
  ApplicationHandler application_handler;
  application_handler.filenames_ = Utilities::buildFilenamesFromDirectory(application_handler, main_dir);
  WorkflowManager workflow_manager;
  LoadSession cs(application_handler, workflow_manager);
  auto& sequenceHandler = application_handler.sequenceHandler_;
  cs.filenames_          = application_handler.filenames_;
  cs.delimiter          = ",";
  cs.checkConsistency   = false;
  cs.process();

  bool result;

  result = InputDataValidation::sampleNamesAreConsistent(sequenceHandler);
  EXPECT_TRUE(result);

  application_handler.filenames_.setFullPath("sequence", main_dir + "/sequence_missing.csv");
  sequenceHandler.clear();

  cs.filenames_ = application_handler.filenames_;
  cs.process();

  result = InputDataValidation::sampleNamesAreConsistent(sequenceHandler);
  EXPECT_FALSE(result); // missing sample: fakeSample
}

TEST(InputDataValidation, componentNamesAreConsistent)
{
  ApplicationHandler application_handler;
  application_handler.filenames_ = Utilities::buildFilenamesFromDirectory(application_handler, main_dir);
  WorkflowManager workflow_manager;
  LoadSession cs(application_handler, workflow_manager);
  auto& sequenceHandler = application_handler.sequenceHandler_;
  cs.filenames_          = application_handler.filenames_;
  cs.delimiter          = ",";
  cs.checkConsistency   = false;
  cs.process();

  bool result;

  result = InputDataValidation::componentNamesAreConsistent(sequenceHandler);
  EXPECT_TRUE(result);

  application_handler.filenames_.setFullPath("traML", main_dir + "/traML_missing.csv");
  // SequenceProcessor::createSequence(sequenceHandler, filenames, ",", false);
  RawDataHandler& rawData0 = sequenceHandler.getSequence().front().getRawData();
  LoadTransitions loadTransitions;
  loadTransitions.process(rawData0, {}, application_handler.filenames_);

  result = InputDataValidation::componentNamesAreConsistent(sequenceHandler);
  EXPECT_FALSE(result);
}

TEST(InputDataValidation, componentNameGroupsAreConsistent)
{
  ApplicationHandler application_handler;
  application_handler.filenames_ = Utilities::buildFilenamesFromDirectory(application_handler, main_dir);
  WorkflowManager workflow_manager;
  LoadSession cs(application_handler, workflow_manager);
  auto& sequenceHandler = application_handler.sequenceHandler_;
  cs.filenames_          = application_handler.filenames_;
  cs.delimiter          = ",";
  cs.checkConsistency   = false;
  cs.process();

  bool result;

  result = InputDataValidation::componentNameGroupsAreConsistent(sequenceHandler);
  EXPECT_TRUE(result);

  application_handler.filenames_.setFullPath("traML", main_dir + "/traML_missing.csv");
  //SequenceProcessor::createSequence(sequenceHandler, filenames, ",", false);
  RawDataHandler& rawData0 = sequenceHandler.getSequence().front().getRawData();
  LoadTransitions loadTransitions;
  loadTransitions.process(rawData0, {}, application_handler.filenames_);

  result = InputDataValidation::componentNameGroupsAreConsistent(sequenceHandler);
  EXPECT_FALSE(result);
}

TEST(InputDataValidation, heavyComponentsAreConsistent)
{
  ApplicationHandler application_handler;
  WorkflowManager workflow_manager;
  application_handler.filenames_ = Utilities::buildFilenamesFromDirectory(application_handler, main_dir);
  LoadSession cs(application_handler, workflow_manager);
  auto& sequenceHandler = application_handler.sequenceHandler_;
  cs.filenames_          = application_handler.filenames_;
  cs.delimiter          = ",";
  cs.checkConsistency   = false;
  cs.process();

  bool result;

  result = InputDataValidation::heavyComponentsAreConsistent(sequenceHandler);
  EXPECT_TRUE(result);

  application_handler.filenames_.setFullPath("quantitationMethods", main_dir + "/quantitationMethods_missing.csv");
  //SequenceProcessor::createSequence(sequenceHandler, filenames, ",", false);
  SequenceSegmentHandler& seqSeg0 = sequenceHandler.getSequenceSegments().front();
  LoadQuantitationMethods loadQuantitationMethods;
  loadQuantitationMethods.process(seqSeg0, SequenceHandler(), {}, application_handler.filenames_);

  result = InputDataValidation::heavyComponentsAreConsistent(sequenceHandler);
  EXPECT_FALSE(result); // g6p.g6p_2.Heavy is missing
}

TEST(InputDataValidation, prepareToLoad)
{
  Filenames filenames;
  filenames.setFullPath("quantitationMethods", main_dir + "/quantitationMethods_missing.csv");
  filenames.setFullPath("non_existing_file", main_dir + "/non_existing_file.csv");
  EXPECT_TRUE(InputDataValidation::prepareToLoad(filenames, "quantitationMethods", true));
  EXPECT_FALSE(InputDataValidation::prepareToLoad(filenames, "non_existing_file", true));
}

TEST(InputDataValidation, prepareToLoadWithBOM)
{
  plog::init(plog::debug, &consoleAppender);
  Filenames filenames;
  filenames.setFullPath("quantitationMethods", main_dir + "/quantitationMethods_bom.csv");
  EXPECT_FALSE(InputDataValidation::prepareToLoad(filenames, "quantitationMethods", true));
  EXPECT_TRUE(InputDataValidation::prepareToLoad(filenames, "quantitationMethods", false));
}

TEST(InputDataValidation, prepareToLoadOneOfTwo)
{
  Filenames filenames;
  filenames.setFullPath("existing_file", main_dir + "/quantitationMethods_missing.csv");
  filenames.setFullPath("non_existing_file", main_dir + "/non_existing_file.csv");
  filenames.setFullPath("empty_file", "");
  filenames.setFullPath("another_empty_file", "");
  EXPECT_TRUE(InputDataValidation::prepareToLoadOneOfTwo(filenames, "existing_file", "empty_file", true));
  EXPECT_TRUE(InputDataValidation::prepareToLoadOneOfTwo(filenames, "empty_file", "existing_file", true));
  EXPECT_FALSE(InputDataValidation::prepareToLoadOneOfTwo(filenames, "empty_file", "another_empty_file", true));
  EXPECT_TRUE(InputDataValidation::prepareToLoadOneOfTwo(filenames, "non_existing_file", "existing_file", true));
  EXPECT_TRUE(InputDataValidation::prepareToLoadOneOfTwo(filenames, "existing_file", "non_existing_file", true));
}

TEST(InputDataValidation, prepareToLoadOneOfTwoWithBOM)
{
  Filenames filenames;
  filenames.setFullPath("existing_file_bom", main_dir + "/quantitationMethods_bom.csv");
  filenames.setFullPath("non_existing_file", main_dir + "/non_existing_file.csv");
  EXPECT_FALSE(InputDataValidation::prepareToLoadOneOfTwo(filenames, "existing_file_bom", "non_existing_file", true));
  EXPECT_FALSE(InputDataValidation::prepareToLoadOneOfTwo(filenames, "non_existing_file", "existing_file_bom", true));
  EXPECT_TRUE(InputDataValidation::prepareToLoadOneOfTwo(filenames, "existing_file_bom", "non_existing_file", false));
  EXPECT_TRUE(InputDataValidation::prepareToLoadOneOfTwo(filenames, "non_existing_file", "existing_file_bom", false));
}

TEST(InputDataValidation, prepareToStore)
{
  Filenames filenames;
  filenames.setFullPath("quantitationMethods", main_dir + "/quantitationMethods_missing.csv");
  EXPECT_TRUE(InputDataValidation::prepareToStore(filenames, "quantitationMethods"));
  filenames.setFullPath("empty_file", "");
  EXPECT_FALSE(InputDataValidation::prepareToStore(filenames, "empty_file"));
}

TEST(InputDataValidation, prepareToStoreOneOfTwo)
{
  Filenames filenames;
  filenames.setFullPath("quantitationMethods", main_dir + "/quantitationMethods_missing.csv");
  filenames.setFullPath("empty_file", "");
  filenames.setFullPath("another_empty_file", "");
  EXPECT_TRUE(InputDataValidation::prepareToStoreOneOfTwo(filenames, "quantitationMethods", "empty_file"));
  EXPECT_FALSE(InputDataValidation::prepareToStoreOneOfTwo(filenames, "empty_file", "another_empty_file"));
}
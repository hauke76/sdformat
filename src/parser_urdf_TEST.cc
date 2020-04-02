/*
 * Copyright 2012 Open Source Robotics Foundation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include <gtest/gtest.h>

#include <list>

#include "sdf/sdf.hh"
#include "sdf/parser_urdf.hh"

/////////////////////////////////////////////////
std::string getMinimalUrdfTxt()
{
  std::ostringstream stream;
  stream << "<robot name='test_robot'>"
         << "  <link name='link1' />"
         << "</robot>";
  return stream.str();
}

/////////////////////////////////////////////////
std::string convertUrdfStrToSdfStr(const std::string &_urdf)
{
  SDF_SUPPRESS_DEPRECATED_BEGIN
  sdf::URDF2SDF parser_;
  SDF_SUPPRESS_DEPRECATED_END
  TiXmlDocument sdf_result = parser_.InitModelString(_urdf);
  TiXmlPrinter printer;
  sdf_result.Accept(&printer);
  return printer.Str();
}

/////////////////////////////////////////////////
void convertUrdfStrToSdf(const std::string &_urdf, sdf::SDF &_sdf)
{
  _sdf.SetFromString(convertUrdfStrToSdfStr(_urdf));
}

/////////////////////////////////////////////////
/* By design, errors are only reported in std output */
TEST(URDFParser, InitModelDoc_EmptyDoc_NoThrow)
{
  // Suppress deprecation for sdf::URDF2SDF
  SDF_SUPPRESS_DEPRECATED_BEGIN
  ASSERT_NO_THROW(
    TiXmlDocument doc = TiXmlDocument();
    sdf::URDF2SDF parser_;
    TiXmlDocument sdf_result = parser_.InitModelDoc(&doc);
  );    // NOLINT(whitespace/parens)
  SDF_SUPPRESS_DEPRECATED_END
}

/////////////////////////////////////////////////
TEST(URDFParser, InitModelDoc_BasicModel_NoThrow)
{
  // Suppress deprecation for sdf::URDF2SDF
  SDF_SUPPRESS_DEPRECATED_BEGIN
  ASSERT_NO_THROW(
    TiXmlDocument doc;
    doc.Parse(getMinimalUrdfTxt().c_str());
    sdf::URDF2SDF parser_;
    TiXmlDocument sdf_result = parser_.InitModelDoc(&doc);
  );    // NOLINT(whitespace/parens)
  SDF_SUPPRESS_DEPRECATED_END
}

/////////////////////////////////////////////////
TEST(URDFParser, ParseResults_BasicModel_ParseEqualToModel)
{
  // URDF -> SDF
  TiXmlDocument doc;
  doc.Parse(getMinimalUrdfTxt().c_str());
  SDF_SUPPRESS_DEPRECATED_BEGIN
  sdf::URDF2SDF parser_;
  SDF_SUPPRESS_DEPRECATED_END
  TiXmlDocument sdf_result = parser_.InitModelDoc(&doc);
  std::string sdf_result_str;
  sdf_result_str << sdf_result;

  // SDF -> SDF
  std::ostringstream stream;
  // parser_urdf.cc exports version "1.7"
  stream << "<sdf version='" << "1.7" << "'>"
         << "  <model name='test_robot' />"
         << "</sdf>";
  TiXmlDocument sdf_doc;
  sdf_doc.Parse(stream.str().c_str());
  std::string sdf_same_result_str;
  sdf_same_result_str << sdf_doc;

  ASSERT_EQ(sdf_same_result_str, sdf_result_str);
}

/////////////////////////////////////////////////
TEST(URDFParser, ParseRobotOriginXYZBlank)
{
  std::ostringstream stream;
  stream << "<robot name=\"test\">"
         << "  <origin />"
         << "  <link name=\"link\" />"
         << "</robot>";
  TiXmlDocument doc;
  doc.Parse(stream.str().c_str());
  SDF_SUPPRESS_DEPRECATED_BEGIN
  sdf::URDF2SDF parser_;
  SDF_SUPPRESS_DEPRECATED_END
  TiXmlDocument sdf_result = parser_.InitModelDoc(&doc);
  TiXmlElement *sdf = sdf_result.FirstChildElement("sdf");
  ASSERT_NE(nullptr, sdf);
  TiXmlElement *model = sdf->FirstChildElement("model");
  ASSERT_NE(nullptr, model);
  TiXmlElement *pose = model->FirstChildElement("pose");
  ASSERT_NE(nullptr, pose);
}

/////////////////////////////////////////////////
TEST(URDFParser, ParseRobotOriginRPYBlank)
{
  std::ostringstream stream;
  stream << "<robot name=\"test\">"
         << "  <origin xyz=\"0 0 0\"/>"
         << "  <link name=\"link\" />"
         << "</robot>";
  TiXmlDocument doc;
  SDF_SUPPRESS_DEPRECATED_BEGIN
  sdf::URDF2SDF parser_;
  SDF_SUPPRESS_DEPRECATED_END
  doc.Parse(stream.str().c_str());
  TiXmlDocument sdf_result = parser_.InitModelDoc(&doc);
  TiXmlElement *sdf = sdf_result.FirstChildElement("sdf");
  ASSERT_NE(nullptr, sdf);
  TiXmlElement *model = sdf->FirstChildElement("model");
  ASSERT_NE(nullptr, model);
  TiXmlElement *pose = model->FirstChildElement("pose");
  ASSERT_NE(nullptr, pose);
}

/////////////////////////////////////////////////
TEST(URDFParser, ParseRobotMaterialBlank)
{
  std::ostringstream stream;
  stream << "<robot name=\"test\">"
         << "  <link name=\"link\">"
         << "    <inertial>"
         << "      <mass value=\"1\"/>"
         << "      <inertia ixx=\"1\" ixy=\"0.0\" ixz=\"0.0\""
         << "               iyy=\"1\" iyz=\"0.0\" izz=\"1\"/>"
         << "    </inertial>"
         << "    <visual>"
         << "      <geometry>"
         << "        <sphere radius=\"1.0\"/>"
         << "      </geometry>"
         << "    </visual>"
         << "  </link>"
         << "  <gazebo reference=\"link\">"
         << "    <mu1>0.2</mu1>"
         << "  </gazebo>"
         << "</robot>";
  TiXmlDocument doc;
  doc.Parse(stream.str().c_str());
  SDF_SUPPRESS_DEPRECATED_BEGIN
  sdf::URDF2SDF parser;
  SDF_SUPPRESS_DEPRECATED_END
  auto sdfXml = parser.InitModelDoc(&doc);
  auto sdfElem = sdfXml.FirstChildElement("sdf");
  ASSERT_NE(nullptr, sdfElem);
  auto modelElem = sdfElem->FirstChildElement("model");
  ASSERT_NE(nullptr, modelElem);
  auto linkElem = modelElem->FirstChildElement("link");
  ASSERT_NE(nullptr, linkElem);
  auto visualElem = linkElem->FirstChildElement("visual");
  ASSERT_NE(nullptr, visualElem);

  auto materialElem = visualElem->FirstChildElement("material");
  ASSERT_EQ(nullptr, materialElem);
}

/////////////////////////////////////////////////
TEST(URDFParser, ParseRobotMaterialName)
{
  std::ostringstream stream;
  stream << "<robot name=\"test\">"
         << "  <link name=\"link\">"
         << "    <inertial>"
         << "      <mass value=\"1\"/>"
         << "      <inertia ixx=\"1\" ixy=\"0.0\" ixz=\"0.0\""
         << "               iyy=\"1\" iyz=\"0.0\" izz=\"1\"/>"
         << "    </inertial>"
         << "    <visual>"
         << "      <geometry>"
         << "        <sphere radius=\"1.0\"/>"
         << "      </geometry>"
         << "    </visual>"
         << "  </link>"
         << "  <gazebo reference=\"link\">"
         << "    <material>Gazebo/Orange</material>"
         << "  </gazebo>"
         << "</robot>";
  TiXmlDocument doc;
  doc.Parse(stream.str().c_str());
  SDF_SUPPRESS_DEPRECATED_BEGIN
  sdf::URDF2SDF parser;
  SDF_SUPPRESS_DEPRECATED_END
  auto sdfXml = parser.InitModelDoc(&doc);
  auto sdfElem = sdfXml.FirstChildElement("sdf");
  ASSERT_NE(nullptr, sdfElem);
  auto modelElem = sdfElem->FirstChildElement("model");
  ASSERT_NE(nullptr, modelElem);
  auto linkElem = modelElem->FirstChildElement("link");
  ASSERT_NE(nullptr, linkElem);
  auto visualElem = linkElem->FirstChildElement("visual");
  ASSERT_NE(nullptr, visualElem);

  auto materialElem = visualElem->FirstChildElement("material");
  ASSERT_NE(nullptr, materialElem);
  auto scriptElem = materialElem->FirstChildElement("script");
  ASSERT_NE(nullptr, scriptElem);
  auto nameElem = scriptElem->FirstChildElement("name");
  ASSERT_NE(nullptr, nameElem);
  EXPECT_EQ("Gazebo/Orange", std::string(nameElem->GetText()));
  auto uriElem = scriptElem->FirstChildElement("uri");
  ASSERT_NE(nullptr, uriElem);
  EXPECT_EQ("file://media/materials/scripts/gazebo.material",
      std::string(uriElem->GetText()));
}

/////////////////////////////////////////////////
TEST(URDFParser, ParseRobotOriginInvalidXYZ)
{
  std::ostringstream stream;
  stream << "<robot name=\"test\">"
         << "  <origin xyz=\"0 foo 0\" rpy=\"0 0 0\"/>"
         << "  <link name=\"link\" />"
         << "</robot>";
  TiXmlDocument doc;
  SDF_SUPPRESS_DEPRECATED_BEGIN
  sdf::URDF2SDF parser_;
  SDF_SUPPRESS_DEPRECATED_END
  doc.Parse(stream.str().c_str());
  TiXmlDocument sdf_result = parser_.InitModelDoc(&doc);
  TiXmlElement *sdf = sdf_result.FirstChildElement("sdf");
  ASSERT_NE(nullptr, sdf);
  TiXmlElement *model = sdf->FirstChildElement("model");
  ASSERT_NE(nullptr, model);
  TiXmlElement *pose = model->FirstChildElement("pose");
  ASSERT_NE(nullptr, pose);
}

/////////////////////////////////////////////////
TEST(URDFParser, ParseGazeboLinkFactors)
{
  std::multimap<std::string, std::vector<std::string>> elements
  {
    {"dampingFactor", {"model", "link", "velocity_decay", "linear", "25.2"}},
    {"dampingFactor", {"model", "link", "velocity_decay", "angular", "25.2"}},
    {"maxVel", {"model", "link", "collision", "surface", "contact", "ode",
                "max_vel", "1.7"}},
    {"minDepth", {"model", "link", "collision", "surface", "contact", "ode",
                  "min_depth", "0.001"}},
    {"mu1", {"model", "link", "collision", "surface", "friction", "ode",
             "mu", "8.9"}},
    {"mu2", {"model", "link", "collision", "surface", "friction", "ode",
             "mu2", "12.5"}},
    {"kp", {"model", "link", "collision", "surface", "contact", "ode",
            "kp", "1000.2"}},
    {"kd", {"model", "link", "collision", "surface", "contact", "ode",
            "kd", "100.1"}},
    {"maxContacts", {"model", "link", "collision", "max_contacts", "99"}},
    {"laserRetro", {"model", "link", "collision", "laser_retro", "72.8"}},
  };

  for (std::map<std::string, std::vector<std::string> >::iterator it =
         elements.begin(); it != elements.end(); ++it)
  {
    std::string value = it->second[it->second.size() - 1];
    std::ostringstream stream;
    stream << "<robot name=\"test\">"
           << "  <link name=\"wheel_left_link\">"
           << "    <collision>"
           << "      <geometry>"
           << "        <cylinder length=\"0.0206\" radius=\"0.0352\"/>"
           << "      </geometry>"
           << "      <origin rpy=\"0 0 0\" xyz=\"0 0 0\"/>"
           << "    </collision>"
           << "    <inertial>"
           << "      <mass value=\"0.01\" />"
           << "      <origin xyz=\"0 0 0\" />"
           << "      <inertia ixx=\"0.001\" ixy=\"0.0\" ixz=\"0.0\""
           << "               iyy=\"0.001\" iyz=\"0.0\""
           << "               izz=\"0.001\" />"
           << "    </inertial>"
           << "  </link>"
           << "  <gazebo reference=\"wheel_left_link\">"
           << "    <" << it->first << ">" << value << "</" << it->first << ">"
           << "  </gazebo>"
           << "</robot>";

    TiXmlDocument doc;
    SDF_SUPPRESS_DEPRECATED_BEGIN
    sdf::URDF2SDF parser_;
    SDF_SUPPRESS_DEPRECATED_END
    doc.Parse(stream.str().c_str());
    TiXmlDocument sdf_result = parser_.InitModelDoc(&doc);

    TiXmlElement *tmp = sdf_result.FirstChildElement("sdf");
    ASSERT_NE(nullptr, tmp);

    unsigned int i;

    for (i = 0; i < it->second.size() - 1; ++i)
    {
      tmp = tmp->FirstChildElement(it->second[i]);
      ASSERT_NE(nullptr, tmp);
    }

    // For the last element, check that it is exactly what we expect
    EXPECT_EQ(tmp->FirstChild()->ValueStr(), it->second[i]);
  }
}

/////////////////////////////////////////////////
TEST(URDFParser, ParseGazeboInvalidDampingFactor)
{
  std::ostringstream stream;
  stream << "<robot name=\"test\">"
         << "  <origin xyz=\"0 0 0\" rpy=\"0 0 0\"/>"
         << "  <link name=\"link\">"
         << "    <inertial>"
         << "      <mass value=\"0.5\" />"
         << "    </inertial>"
         << "  </link>"
         << "  <gazebo reference=\"link\">"
         << "    <dampingFactor>foo</dampingFactor>"
         << "  </gazebo>"
         << "</robot>";
  TiXmlDocument doc;
  SDF_SUPPRESS_DEPRECATED_BEGIN
  sdf::URDF2SDF parser_;
  SDF_SUPPRESS_DEPRECATED_END
  doc.Parse(stream.str().c_str());
  ASSERT_THROW(TiXmlDocument sdf_result = parser_.InitModelDoc(&doc),
               std::invalid_argument);
}

/////////////////////////////////////////////////
TEST(URDFParser, ParseGazeboJointElements)
{
  std::map<std::string, std::vector<std::string>> elements
  {
    {"stopCfm", {"model", "joint", "physics", "ode", "limit", "cfm", "0.8"}},
    {"stopErp", {"model", "joint", "physics", "ode", "limit", "erp", "0.8"}},
    {"fudgeFactor", {"model", "joint", "physics", "ode", "fudge_factor",
        "11.1"}},
  };

  for (std::map<std::string, std::vector<std::string> >::iterator it =
         elements.begin(); it != elements.end(); ++it)
  {
    std::string value = it->second[it->second.size() - 1];
    std::ostringstream stream;
    stream << "<robot name=\"test\">"
           << "  <link name=\"chest_link\">"
           << "    <inertial>"
           << "      <mass value=\"10\" />"
           << "      <origin xyz=\"0 0 -0.1\" />"
           << "      <inertia ixx=\"1E-4\""
           << "               iyy=\"1E-4\""
           << "               izz=\"1E-4\""
           << "               ixy=\"1E-7\""
           << "               ixz=\"1E-7\""
           << "               iyz=\"1E-7\"/>"
           << "    </inertial>"
           << "    <collision>"
           << "      <origin xyz=\"0 0 -0.2337\" rpy=\"0.0 0.0 0.0\" />"
           << "    </collision>"
           << "  </link>"
           << "  <link name=\"neck_tilt\">"
           << "    <inertial>"
           << "      <mass value=\"0.940\" />"
           << "      <origin xyz=\"0.000061 0.003310 0.028798\"/>"
           << "      <inertia ixx=\"0.001395\""
           << "               iyy=\"0.001345\""
           << "               izz=\"0.000392\""
           << "               ixy=\"-0.000000\""
           << "               ixz=\"-0.000000\""
           << "               iyz=\"-0.000085\" />"
           << "    </inertial>"
           << "    <collision>"
           << "      <origin xyz=\"0 0 0\" rpy=\"0.0 0.0 0.0\" />"
           << "    </collision>"
           << "  </link>"
           << "  <joint name=\"head_j0\" type=\"revolute\">"
           << "    <axis xyz=\"0 -1 0\" />"
           << "    <origin xyz=\"0.0 0.0 0.07785\" rpy=\"0 0 0\" />"
           << "    <parent link=\"chest_link\"/>"
           << "    <child link=\"neck_tilt\"/>"
           << "    <limit effort=\"100\""
           << "           velocity=\"0.349\""
           << "           lower=\"-0.314\""
           << "           upper=\"0.017\"/>"
           << "  </joint>"
           << "  <gazebo reference=\"head_j0\">"
           << "    <" << it->first << ">" << value << "</" << it->first << ">"
           << "  </gazebo>"
           << "</robot>";

    TiXmlDocument doc;
    SDF_SUPPRESS_DEPRECATED_BEGIN
    sdf::URDF2SDF parser_;
    SDF_SUPPRESS_DEPRECATED_END
    doc.Parse(stream.str().c_str());
    TiXmlDocument sdf_result = parser_.InitModelDoc(&doc);

    TiXmlElement *tmp = sdf_result.FirstChildElement("sdf");
    ASSERT_NE(nullptr, tmp);

    unsigned int i;

    for (i = 0; i < it->second.size() - 1; ++i)
    {
      tmp = tmp->FirstChildElement(it->second[i]);
      ASSERT_NE(nullptr, tmp);
    }

    // For the last element, check that it is exactly what we expect
    EXPECT_EQ(tmp->FirstChild()->ValueStr(), it->second[i]);
  }
}

/////////////////////////////////////////////////
TEST(URDFParser, CheckFixedJointOptions_NoOption)
{
  // Convert a fixed joint with no options (i.e. it will be lumped)
  std::ostringstream fixedJointNoOptions;
  fixedJointNoOptions << "<robot name='test_robot'>"
    << "  <link name='link1'>"
    << "    <inertial>"
    << "      <origin xyz='0.0 0.0 0.0' rpy='0.0 0.0 0.0'/>"
    << "      <mass value='1.0'/>"
    << "      <inertia ixx='1.0' ixy='0.0' ixz='0.0'"
    << "               iyy='1.0' iyz='0.0' izz='1.0'/>"
    << "    </inertial>"
    << "  </link>"
    << "  <link name='link2'>"
    << "    <inertial>"
    << "      <origin xyz='0.0 0.0 0.0' rpy='0.0 0.0 0.0'/>"
    << "      <mass value='1.0'/>"
    << "      <inertia ixx='1.0' ixy='0.0' ixz='0.0'"
    << "               iyy='1.0' iyz='0.0' izz='1.0'/>"
    << "    </inertial>"
    << "  </link>"
    << "  <joint name='joint1_2' type='fixed'>"
    << "    <parent link='link1' />"
    << "    <child  link='link2' />"
    << "    <origin xyz='0.0 0.0 0.0' rpy='0.0 0.0 0.0' />"
    << "  </joint>"
    << "</robot>";

  // Check that there are no joints in the converted SDF
  sdf::SDF fixedJointNoOptionsSDF;
  convertUrdfStrToSdf(fixedJointNoOptions.str(), fixedJointNoOptionsSDF);
  sdf::ElementPtr elem = fixedJointNoOptionsSDF.Root();
  ASSERT_NE(nullptr, elem);
  ASSERT_TRUE(elem->HasElement("model"));
  elem = elem->GetElement("model");
  ASSERT_FALSE(elem->HasElement("joint"));
}

/////////////////////////////////////////////////
TEST(URDFParser, CheckFixedJointOptions_disableJointLumping)
{
  // Convert a fixed joint with disableJointLumping
  // (i.e. converted to fake revolute joint)
  std::ostringstream fixedJointDisableJointLumping;
  fixedJointDisableJointLumping << "<robot name='test_robot'>"
    << "  <link name='link1'>"
    << "    <inertial>"
    << "      <origin xyz='0.0 0.0 0.0' rpy='0.0 0.0 0.0'/>"
    << "      <mass value='1.0'/>"
    << "      <inertia ixx='1.0' ixy='0.0' ixz='0.0'"
    << "               iyy='1.0' iyz='0.0' izz='1.0'/>"
    << "    </inertial>"
    << "  </link>"
    << "  <link name='link2'>"
    << "    <inertial>"
    << "      <origin xyz='0.0 0.0 0.0' rpy='0.0 0.0 0.0'/>"
    << "      <mass value='1.0'/>"
    << "      <inertia ixx='1.0' ixy='0.0' ixz='0.0'"
    << "               iyy='1.0' iyz='0.0' izz='1.0'/>"
    << "    </inertial>"
    << "  </link>"
    << "  <joint name='joint1_2' type='fixed'>"
    << "    <parent link='link1' />"
    << "    <child  link='link2' />"
    << "    <origin xyz='0.0 0.0 0.0' rpy='0.0 0.0 0.0' />"
    << "  </joint>"
    << "  <gazebo reference='joint1_2'>"
    << "    <disableFixedJointLumping>true</disableFixedJointLumping>"
    << "  </gazebo>"
    << "</robot>";

  // Check that there is a revolute joint in the converted SDF
  sdf::SDF fixedJointDisableJointLumpingSDF;
  convertUrdfStrToSdf(fixedJointDisableJointLumping.str(),
      fixedJointDisableJointLumpingSDF);
  sdf::ElementPtr elem = fixedJointDisableJointLumpingSDF.Root();
  ASSERT_NE(nullptr, elem);
  ASSERT_TRUE(elem->HasElement("model"));
  elem = elem->GetElement("model");
  ASSERT_TRUE(elem->HasElement("joint"));
  elem = elem->GetElement("joint");
  std::string jointType = elem->Get<std::string>("type");
  ASSERT_EQ(jointType, "revolute");
}

/////////////////////////////////////////////////
TEST(URDFParser, CheckFixedJointOptions_preserveFixedJoint)
{
  // Convert a fixed joint with only preserveFixedJoint
  // (i.e. converted to fixed joint)
  std::ostringstream fixedJointPreserveFixedJoint;
  fixedJointPreserveFixedJoint << "<robot name='test_robot'>"
    << "  <link name='link1'>"
    << "    <inertial>"
    << "      <origin xyz='0.0 0.0 0.0' rpy='0.0 0.0 0.0'/>"
    << "      <mass value='1.0'/>"
    << "      <inertia ixx='1.0' ixy='0.0' ixz='0.0'"
    << "               iyy='1.0' iyz='0.0' izz='1.0'/>"
    << "    </inertial>"
    << "  </link>"
    << "  <link name='link2'>"
    << "    <inertial>"
    << "      <origin xyz='0.0 0.0 0.0' rpy='0.0 0.0 0.0'/>"
    << "      <mass value='1.0'/>"
    << "      <inertia ixx='1.0' ixy='0.0' ixz='0.0'"
    << "               iyy='1.0' iyz='0.0' izz='1.0'/>"
    << "    </inertial>"
    << "  </link>"
    << "  <joint name='joint1_2' type='fixed'>"
    << "    <parent link='link1' />"
    << "    <child  link='link2' />"
    << "    <origin xyz='0.0 0.0 0.0' rpy='0.0 0.0 0.0' />"
    << "  </joint>"
    << "  <gazebo reference='joint1_2'>"
    << "    <preserveFixedJoint>true</preserveFixedJoint>"
    << "  </gazebo>"
    << "</robot>";

  // Check that there is a fixed joint in the converted SDF
  sdf::SDF fixedJointPreserveFixedJointSDF;
  convertUrdfStrToSdf(fixedJointPreserveFixedJoint.str(),
      fixedJointPreserveFixedJointSDF);
  sdf::ElementPtr elem = fixedJointPreserveFixedJointSDF.Root();
  ASSERT_NE(nullptr, elem);
  ASSERT_TRUE(elem->HasElement("model"));
  elem = elem->GetElement("model");
  ASSERT_TRUE(elem->HasElement("joint"));
  elem = elem->GetElement("joint");
  std::string jointType = elem->Get<std::string>("type");
  ASSERT_EQ(jointType, "fixed");
}

/////////////////////////////////////////////////
TEST(URDFParser,
    CheckFixedJointOptions_preserveFixedJoint_and_disableJointLumping)
{
  // Convert a fixed joint with disableJointLumping and preserveFixedJoint
  // (i.e. converted to fixed joint)
  std::ostringstream fixedJointPreserveFixedJoint;
  fixedJointPreserveFixedJoint << "<robot name='test_robot'>"
    << "  <link name='link1'>"
    << "    <inertial>"
    << "      <origin xyz='0.0 0.0 0.0' rpy='0.0 0.0 0.0'/>"
    << "      <mass value='1.0'/>"
    << "      <inertia ixx='1.0' ixy='0.0' ixz='0.0'"
    << "               iyy='1.0' iyz='0.0' izz='1.0'/>"
    << "    </inertial>"
    << "  </link>"
    << "  <link name='link2'>"
    << "    <inertial>"
    << "      <origin xyz='0.0 0.0 0.0' rpy='0.0 0.0 0.0'/>"
    << "      <mass value='1.0'/>"
    << "      <inertia ixx='1.0' ixy='0.0' ixz='0.0'"
    << "               iyy='1.0' iyz='0.0' izz='1.0'/>"
    << "    </inertial>"
    << "  </link>"
    << "  <joint name='joint1_2' type='fixed'>"
    << "    <parent link='link1' />"
    << "    <child  link='link2' />"
    << "    <origin xyz='0.0 0.0 0.0' rpy='0.0 0.0 0.0' />"
    << "  </joint>"
    << "  <gazebo reference='joint1_2'>"
    << "    <disableFixedJointLumping>true</disableFixedJointLumping>"
    << "  </gazebo>"
    << "  <gazebo reference='joint1_2'>"
    << "    <preserveFixedJoint>true</preserveFixedJoint>"
    << "  </gazebo>"
    << "</robot>";

  // Check that there is a fixed joint in the converted SDF
  sdf::SDF fixedJointPreserveFixedJointSDF;
  convertUrdfStrToSdf(fixedJointPreserveFixedJoint.str(),
      fixedJointPreserveFixedJointSDF);
  sdf::ElementPtr elem = fixedJointPreserveFixedJointSDF.Root();
  ASSERT_NE(nullptr, elem);
  ASSERT_TRUE(elem->HasElement("model"));
  elem = elem->GetElement("model");
  ASSERT_TRUE(elem->HasElement("joint"));
  elem = elem->GetElement("joint");
  std::string jointType = elem->Get<std::string>("type");
  ASSERT_EQ(jointType, "fixed");
}

/////////////////////////////////////////////////
TEST(URDFParser, CheckFixedJointOptions_NoOption_Repeated)
{
  // Convert a fixed joint with no options (i.e. it will be lumped)
  // Converting again to make sure that the map
  // joint names ---> lumped options has been correctly reset
  std::ostringstream fixedJointNoOptions;
  fixedJointNoOptions << "<robot name='test_robot'>"
    << "  <link name='link1'>"
    << "    <inertial>"
    << "      <origin xyz='0.0 0.0 0.0' rpy='0.0 0.0 0.0'/>"
    << "      <mass value='1.0'/>"
    << "      <inertia ixx='1.0' ixy='0.0' ixz='0.0'"
    << "               iyy='1.0' iyz='0.0' izz='1.0'/>"
    << "    </inertial>"
    << "  </link>"
    << "  <link name='link2'>"
    << "    <inertial>"
    << "      <origin xyz='0.0 0.0 0.0' rpy='0.0 0.0 0.0'/>"
    << "      <mass value='1.0'/>"
    << "      <inertia ixx='1.0' ixy='0.0' ixz='0.0'"
    << "               iyy='1.0' iyz='0.0' izz='1.0'/>"
    << "    </inertial>"
    << "  </link>"
    << "  <joint name='joint1_2' type='fixed'>"
    << "    <parent link='link1' />"
    << "    <child  link='link2' />"
    << "    <origin xyz='0.0 0.0 0.0' rpy='0.0 0.0 0.0' />"
    << "  </joint>"
    << "</robot>";

  // Check that there are no joints in the converted SDF
  sdf::SDF fixedJointNoOptionsSDF;
  convertUrdfStrToSdf(fixedJointNoOptions.str(), fixedJointNoOptionsSDF);
  sdf::ElementPtr elem = fixedJointNoOptionsSDF.Root();
  ASSERT_NE(nullptr, elem);
  ASSERT_TRUE(elem->HasElement("model"));
  elem = elem->GetElement("model");
  ASSERT_FALSE(elem->HasElement("joint"));
}

/////////////////////////////////////////////////
TEST(URDFParser, CheckJointTransform)
{
  std::stringstream str;
  str.precision(16);
  str << "<robot name='test_robot'>"
    << "  <link name='world'/>"
    << "  <joint name='jointw_1' type='fixed'>"
    << "    <parent link='world' />"
    << "    <child  link='link1' />"
    << "    <origin xyz='0.0 0.0 0.0' rpy='0.0 0.0 0.0' />"
    << "  </joint>"
    << "  <link name='link1'>"
    << "    <inertial>"
    << "      <origin xyz='0.0 0.0 0.0' rpy='0.0 0.0 0.0'/>"
    << "      <mass value='1.0'/>"
    << "      <inertia ixx='1.0' ixy='0.0' ixz='0.0'"
    << "               iyy='1.0' iyz='0.0' izz='1.0'/>"
    << "    </inertial>"
    << "  </link>"
    << "  <joint name='joint1_2' type='continuous'>"
    << "    <parent link='link1' />"
    << "    <child  link='link2' />"
    << "    <origin xyz='0.0 0.0 0.0' rpy='0.0 0.0 " << IGN_PI*0.5 << "' />"
    << "  </joint>"
    << "  <link name='link2'>"
    << "    <inertial>"
    << "      <origin xyz='0.0 0.0 0.0' rpy='0.0 0.0 0.0'/>"
    << "      <mass value='1.0'/>"
    << "      <inertia ixx='1.0' ixy='0.0' ixz='0.0'"
    << "               iyy='1.0' iyz='0.0' izz='1.0'/>"
    << "    </inertial>"
    << "  </link>"
    << "  <joint name='joint2_3' type='continuous'>"
    << "    <parent link='link2' />"
    << "    <child  link='link3' />"
    << "    <origin xyz='1.0 0.0 0.0' rpy='0.0 0.0 0.0' />"
    << "  </joint>"
    << "  <link name='link3'>"
    << "    <inertial>"
    << "      <origin xyz='0.0 0.0 0.0' rpy='0.0 0.0 0.0'/>"
    << "      <mass value='1.0'/>"
    << "      <inertia ixx='1.0' ixy='0.0' ixz='0.0'"
    << "               iyy='1.0' iyz='0.0' izz='1.0'/>"
    << "    </inertial>"
    << "  </link>"
    << "</robot>";

  std::string expectedSdf = R"(<sdf version="1.7">
    <model name="test_robot">
        <joint type="fixed" name="jointw_1">
            <pose relative_to="__model__">0 0 0 0 0 0</pose>
            <child>link1</child>
            <parent>world</parent>
        </joint>
        <link name="link1">
            <pose relative_to="jointw_1" />
            <inertial>
                <pose>0 0 0 0 0 0</pose>
                <mass>1</mass>
                <inertia>
                    <ixx>1</ixx>
                    <ixy>0</ixy>
                    <ixz>0</ixz>
                    <iyy>1</iyy>
                    <iyz>0</iyz>
                    <izz>1</izz>
                </inertia>
            </inertial>
        </link>
        <joint type="revolute" name="joint1_2">
            <pose relative_to="link1">0 0 0 0 0 1.570796326794897</pose>
            <child>link2</child>
            <parent>link1</parent>
            <axis>
                <xyz>1 0 0</xyz>
                <limit />
                <dynamics />
            </axis>
        </joint>
        <link name="link2">
            <pose relative_to="joint1_2" />
            <inertial>
                <pose>0 0 0 0 0 0</pose>
                <mass>1</mass>
                <inertia>
                    <ixx>1</ixx>
                    <ixy>0</ixy>
                    <ixz>0</ixz>
                    <iyy>1</iyy>
                    <iyz>0</iyz>
                    <izz>1</izz>
                </inertia>
            </inertial>
        </link>
        <joint type="revolute" name="joint2_3">
            <pose relative_to="link2">1 0 0 0 0 0</pose>
            <child>link3</child>
            <parent>link2</parent>
            <axis>
                <xyz>1 0 0</xyz>
                <limit />
                <dynamics />
            </axis>
        </joint>
        <link name="link3">
            <pose relative_to="joint2_3" />
            <inertial>
                <pose>0 0 0 0 0 0</pose>
                <mass>1</mass>
                <inertia>
                    <ixx>1</ixx>
                    <ixy>0</ixy>
                    <ixz>0</ixz>
                    <iyy>1</iyy>
                    <iyz>0</iyz>
                    <izz>1</izz>
                </inertia>
            </inertial>
        </link>
    </model>
</sdf>
)";

  std::string sdfStr = convertUrdfStrToSdfStr(str.str());
  EXPECT_EQ(expectedSdf, sdfStr);
}
/////////////////////////////////////////////////
TEST(URDFParser, OutputPrecision)
{
  std::string str = R"(
    <robot name='test_robot'>
      <link name='link1'>
          <inertial>
            <mass value="0.1" />
            <origin rpy="1.570796326794895 0 0" xyz="0.123456789123456 0 0.0" />
            <inertia ixx="0.01" ixy="0" ixz="0" iyy="0.01" iyz="0" izz="0.01" />
          </inertial>
        </link>
    </robot>)";

  SDF_SUPPRESS_DEPRECATED_BEGIN
  sdf::URDF2SDF parser;
  SDF_SUPPRESS_DEPRECATED_END
  TiXmlDocument sdfResult = parser.InitModelString(str);

  auto root = sdfResult.RootElement();
  auto model = root->FirstChild("model");
  ASSERT_NE(nullptr, model);
  auto link = model->FirstChild("link");
  ASSERT_NE(nullptr, link);
  auto inertial = link->FirstChild("inertial");
  ASSERT_NE(nullptr, inertial);
  auto pose = inertial->FirstChild("pose");
  ASSERT_NE(nullptr, pose);
  ASSERT_NE(nullptr, pose->FirstChild());
  std::string poseTxt = pose->FirstChild()->ValueStr();
  EXPECT_FALSE(poseTxt.empty());

  std::string poseValues[6];
  std::istringstream ss(poseTxt);

  for (int i = 0; i < 6; ++i)
  {
    ss >> poseValues[i];
  }

  // Check output precision
  EXPECT_EQ("0.123456789123456", poseValues[0]);
  EXPECT_EQ("1.570796326794895", poseValues[3]);

  // Check that 0 doesn't get printed as -0
  EXPECT_EQ("0", poseValues[1]);
  EXPECT_EQ("0", poseValues[2]);
  EXPECT_EQ("0", poseValues[4]);
  EXPECT_EQ("0", poseValues[5]);
}

/////////////////////////////////////////////////
/// Main
int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

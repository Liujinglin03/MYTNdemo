// Copyright 2018 Slightech Co., Ltd. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#include <fstream>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "mynteye/logger.h"
#include "mynteye/device/device.h"
#include "mynteye/device/utils.h"
#include "mynteye/util/times.h"

#include "dataset.h"

MYNTEYE_USE_NAMESPACE

int main(int argc, char *argv[]) {
  glog_init _(argc, argv);

  auto &&api = API::Create(argc, argv);
  if (!api) return 1;

  auto request = api->GetStreamRequest();

//   struct StreamRequest {
//   /** Stream width in pixels */
//   std::uint16_t width;
//   /** Stream height in pixels */
//   std::uint16_t height;
//   /** Stream pixel format */
//   Format format;
//   /** Stream frames per second */ 
//   std::uint16_t fps;
//   }

  request.fps = 60;
  api->ConfigStreamRequest(request);
  api->EnableStreamData(Stream::LEFT_RECTIFIED);
  api->EnableStreamData(Stream::RIGHT_RECTIFIED);
  api->Start(Source::VIDEO_STREAMING);

  const char *outdir;
  if (argc >= 2) {
    outdir = argv[1];
  } else {
    outdir = "./dataset";
  }
  //tools::Dataset dataset(outdir);
  cv::namedWindow("frame");
  int img_count = 0;
  char leftName[150] = {};
  char rightName[150] = {};
  while (true) {
	  api->WaitForStreams();

	  auto &&left_data = api->GetStreamData(Stream::LEFT_RECTIFIED);
	  auto &&right_data = api->GetStreamData(Stream::RIGHT_RECTIFIED);
	 
	  if (!left_data.frame.empty() && !right_data.frame.empty()) {
		  cv::Mat img;
		  cv::hconcat(left_data.frame, right_data.frame, img);
		  cv::imshow("frame", img);
		  sprintf(leftName, "./dataset/left/%04d.png", img_count);
		  sprintf(rightName, "./dataset/right/%04d.png", img_count);
		  //cv::imwrite(leftName, left_data.frame);
		  //cv::imwrite(rightName, right_data.frame);
		  img_count++;
		  cv::waitKey(0);
  	  }
	  
	  char key = static_cast<char>(cv::waitKey(1));
	  if (key == 27 || key == 'q' || key == 'Q') {  // ESC/Q
		  break;
	  }
  }
  

	api->Stop(Source::VIDEO_STREAMING);
	return 0;
}
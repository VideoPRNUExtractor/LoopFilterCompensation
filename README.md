# Loop Filter Compensation
Loop filter compensation for H264 and HEVC coded videos. See Ref [1-2] for more details. The main capability of the tool is compensation for the effects of loop filtering procedure applied at encoder and decoder.

It must be noted that for stabilized videos loop filter compensation must be applied as defined in Ref [3].


Extractor:  Extracts loop filter compensated video frames

FFMPEG:  	modified FFMPEG libraries

## Requirements
- This project works on the Linux architecture. The code was tested on Ubuntu version 18.04
- Check if you have Java installed on your system with `java -version`
- If you do not have java, install it by running `sudo apt install default-jre` command.
- Clone git repository: `git clone https://github.com/VideoPRNUExtractor/LoopFilterCompensation.git`

## FFMPEG Installation
- Change your working directory to FFMPEG directory. `cd LoopFilterCompensation/FFMPEG/`
- Install H264 library by running `sudo apt-get install libx264-dev`
- HEVC installation can be a problem with default apt-get therefor first download the latest version of x265 from [link](http://ftp.videolan.org/pub/videolan/x265/ "link")
	- Extract tar file and change your working directory to x265_your_version. `cd x265_1.5`
	- Install Yasm with `sudo apt-get install yasm`
	- Install cmake with `sudo apt-get install mercurial cmake cmake-curses-gui build-essential yasm`
	- Configure to x265 by running `./make-Makefiles.bash`; after that press c for configure then g for generate.
	- Run `make` command.
 		- Some versions interestingly contain string error in two files. If you see that you can change this line according to GCC output.
	- If you have libx265 remove it by running `sudo apt-get autoremove libx265-dev`
	- Install x265 with `sudo make install`
	- Update to library folders by running `sudo ldconfig`
- Change your working directory to FFMPEG directory.
- Install package config with running `sudo apt install pkg-config`
- Configure your FFMPEG by running ` ./configure --disable-optimizations --disable-mmx --disable-stripping --enable-debug=3 --disable-asm --extra-cflags="-gstabs+" --enable-libx264 --enable-gpl --enable-libx265`
- Run `sudo make examples`
- Run `sudo make install`
 - If you change any code in FFMPEG you must run this code again.

### Testing FFMPEG
- You can download a raw video from [link](https://media.xiph.org/video/derf/ "link")
- You can use downloaded video as a input file to create a video with running `ffmpeg -i football_cif.y4m  -c:v libx264 -preset slow -crf 22 -c:a copy  -vframes 120 football_cif.mkv`

------------

## Installation of Extractor

- Change your folder to the Extractor folder.
- Run `sudo make install`

## Usage

Extractor takes a video path as an input.

- This function creates a folder with appended a "Frame" string to video's path. This folder contains the loop filter compensated frames of video.
- The usage of extractor is `./extract_mvs video_path`.


## References

Ref [1]: Altinisik, Enes, Kasim Tasdemir, and Husrev Taha Sencar. "Extracting PRNU Noise from H.264 Coded Videos." 2018 26th European signal processing conference (EUSIPCO). IEEE, 2018 [.PDF](https://ieeexplore.ieee.org/iel7/8537458/8552938/08553173.pdf ".PDF")

Ref [2]: Altinisik, Enes, Kasim Tasdemir, and Husrev Taha Sencar. "Mitigation of H.264 and H.265 Video Compression for Reliable PRNU Estimation." IEEE Transactions on information forensics and security 15 (2019) [.PDF](https://ieeexplore.ieee.org/document/8854840 ".PDF")

Ref [3]: Altinisik, Enes, and Husrev Taha Sencar. "Source Camera Verification for Strongly Stabilized Videos," IEEE Transactions on information forensics and security (2020) [.PDF](https://ieeexplore.ieee.org/document/9169924 ".PDF")

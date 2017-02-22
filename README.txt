This is a VS 2015 solution that enables the building of 
Windows 8.1 WinRT applications with access to MuPDF.

To make use of this project you should:

1) Do a git checkout of the mupdf submodule. The mupdf submodule also has submodules
   of its own that you will need to checkout.

2) Open the mupdf solution in  
   .\submodules\mupdf\platform\win32\mupdf.sln with VS 2015 allowing VS to convert the
   projects to VS 2015 formats.   Once the conversion is completed, close this solution.

3) Create the winRT projects from the mupdf projects.  This is done by running
   winrt_project_setup.bat
   
4) Open the VS 2015 solution mupdfwinrt.sln

5) If you wish to do builds for the ARM processor, you will want to create new configurations
   for libfonts_winrt, libmupdf_winrt, and libthirdparty_winrt.  This is done by selecting
   the one of these, and going to properties.  Once the Property Pages window open, press the
   Configuration Manager... button.   Under the Platform column, for libfonts_winrt, libmupdf_winrt
   and libthirdparty_winrt select <New..>.  Under New Platform, select ARM and under Copy settings
   from select Win32.  Make sure the box "Create new solution platforms" is left unchecked.

If you follow these steps, you should be able to build for x64, win32 and ARM creating the DLL
mupdfwinrt.dll which you can then call into from your C#, C++, or Javascript winRT application.
The winRT API is defined in mudocument.cpp/h 

You will need to add the dll as a reference into your application.  Making sure to include ../mupdfwinrt/
and ../$(Platform)/$(Configuration)/mupdfwinrt.lib as a dependency (if you are building a C++ application)




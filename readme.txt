- Author: Hongxing Yuan (yuanhx@mail.ustc.edu.cn)
- Version: 1.0 (last update: May-21-2014)

- Note:
(1) The implementation is for comparision with our paper:
    Yuan H, Wu S, Cheng P, An P, and Bao S, Nonlocal Random Walks for Semi-Automatic 2D-to-3D Image Conversion, 
    IEEE Signal Processing Letters, 2015, 22(3): 371-374.
(2) This code is reimplemented by Hongxing Yuan based on the Graph-cuts (GC) method of the following paper:
    Semi-Automatic Depth Map Generation in Unconstrained Images and Video Sequences for 2D to Stereoscopic 3D Conversion
(3) We use MAXFLOW 3.01 to perform GC computation which can be downloaded from:
    http://www.cs.ucl.ac.uk/staff/V.Kolmogorov/software.html
    We also use OpenCV 2.4.6 to read and write images, the higher version should also work.
(4) This code has been tested in Microsoft Visual Studio 2012 running on Windows 7(64bit). 
    
- Usage example:
    gc girl/girl.png girl/mask.png girl/strokes.png
  You can use "brush2depth" software to generate "mask.png" and "strokes.png", which can be downloaded from:
  https://github.com/tcyhx/brush2depth
    
- For any questions, suggestions or bug reports, please, contact yuanhx@mail.ustc.edu.cn

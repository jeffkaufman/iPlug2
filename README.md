# Bass Whistle iPlug 2 Plugin
### Fork of iPlug2 which adds Bass Whistle

To use this, follow an iPlug2 tutorial.  To build a VST3 for Reaper you'll need
to download the VST3 SDK and manually copy it into the dependencies directory.

The main code for BassWhistle is in
https://github.com/jeffkaufman/pitch-detect/blob/master/zeros.c  This is a fork
of both that and of iPlug2 which allows it to be used as a VST3 in Reaper.

I should probably make zeros.c more of a library and allow this to include it
as a submodule, but for now the code is copied and tweaked to work in the new
environment.

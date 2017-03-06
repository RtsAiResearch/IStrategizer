BUILD INSTRUCTIONS FOR YARMOUK:
===============================
Environment:
- BWAPI_DIR env variable is used to point to bwapi lib/include dirs.
- The XP Bot Machine should have VS 2013 and VS 2010 VC++ Redistributables installed.
- VS 2013 is required for compiling all the project.

Building:
- Open Source\src\Yarmouk.sln in Visual Studio 2013.
- (IMPORTANT) If VS 2013 prompts to convert any of the projects to a newer version of VS, dismiss the message and ignore it.
- Select 'Release' build configuration for the entire solution.
- Build the solution.
- Yarmouk VS compiled binaries/datafiles will be in Source\engine\Win32Debug as follows: Yarmouk.dll, IStrategizer.dll, IStrategizerEx.cb, Typeinfo folder

Precompiled Binaries are in AI folder for immediate testing, and they should be placed in the AI folder in bwapi-data\AI
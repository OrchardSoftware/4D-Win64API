//%attributes = {}
  // Win64API_main
  //
  // This method will act as a unit test to the Win64API commands and a sample database to show what
  // it can do.
  //
  // ACW 10/3/19 WIN-71

C_BOOLEAN:C305($bMemoryTest)

  // Turn this to true to loop over a command many times to test memory usage.
$bMemoryTest:=False:C215

  // ACW 10/16/20 WIN-71
win_71 ($bMemoryTest)

  // ACW 10/20/20 WIN-78
win_78 ($bMemoryTest)

  // ACW 10/16/20 WIN-80
win_80 ($bMemoryTest)

  // End of demo/unit test; if we haven't hit a TRACE up until now all the commands are working as expected.
TRACE:C157

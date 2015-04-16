This file is to detail the steps to build and run AutoTester. Choose either method 1 or 2 to test SPA. 
To test our bonus features, you can view the files in the test case path 
where there are sample test case files for the bonus features. 

Files needed:
analysis.xsl - Given in the AutoTester guide (http://www.comp.nus.edu.sg/~cs3201/Tools-Lab/AutoTester.html)

Path of AutoTester executable produced:
.\Group05\Release\AutoTester.exe

Path of test cases:
.\Group05\AutoTester\tests\

****************************
****** Build solution ******
****************************

Steps: 
1. Open Group5.sln (e.g. in \Group05\Group5.sln)
2. Ensure that the solution is in "Release" mode
3. Build solution by pressing F7

****************************


****************************
****** Run AutoTester ******
****************************

##### Simplier method -- Method 1 ######

Steps: 
1. Navigate into the Auto Tester's 'tests' Folder (e.g. \Group05\AutoTester\tests)
2. Open AutoTest.bat on Sublime/Notepad++ or any text editor (e.g. \Group05\AutoTester\tests\AutoTest.bat)
3. Set the parameters to the file you want to test. All the test files is located in the 'tests' folder.
	e.g. 	set src=contains\src.txt
		set qry=contains\qry.txt
		set out=output.xml
4. Double click AutoTest.bat, a command prompt execuatable pops up. 
5. Press enter to run.
6. To view the output.xml created, add in analysis.xsl in the same folder as output.xml (i.e. in \Group05\AutoTester\tests\)
7. Use Firefox to open output.xml


##### Tedious method -- Method 2 ##### 

1. Open cmd
2. cd into the Release Folder (e.g. \Group05\Release\)
3. Type in "AutoTester test_directory\source.txt test_directory\queries.txt output.xml"
	where test_directory refers to the directory where the test files are located
4. Note that source.txt and queries.txt are test case files stored in AutoTester\tests (e.g. \Group05\AutoTester\tests\contains)
5. To view the output.xml created, add in analysis.xsl in the same folder as output.xml (i.e. in \Group05\AutoTester\tests\)
6. Use Firefox to open output.xml

****************************

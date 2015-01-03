This file is to detail the steps to run AutoTester. Choose either method 1 or 2 to test SPA. 
To test our bonus features you can test the files in \SPA_repo\AutoTester\tests where there 
are test case files for assign-pattern clause and while-pattern clause queries. 

*****************************
****** Test AutoTester ******
*****************************

##### Simplier method -- Method 1 ######

Steps: 
1. Open Group5.sln (eg in \SPA_repo\Group5.sln)
2. Build solution by pressing on F7
3. Navigate into the Auto Tester's 'tests' Folder (eg \SPA_repo\AutoTester\tests)
4. Open AutoTest.bat on Sublime/Notepad++ (eg \SPA_repo\AutoTester\tests\AutoTest.bat)
5. Modify the file you want to test. All the test files is located in the 'tests' folder.
	eg 	set src=case0\test_src.txt
		set qry=case0\test_qry.txt
		set out=out.xml
6. Double click AutoTest.bat, a command prompt execuatable pops up. 
7. Press enter to run.
8. Press any button to close AutoTest.bat's command prompt. 
9. Open out.xml created on Firefox web browser. 


##### Tedious method -- Method 2 ##### 

1. Navigate into the Debug Folder (eg \SPA_repo\Debug\)
2. Open Group5.sln (eg in \SPA_repo\Group5.sln)
3. Build solution by pressing on F7
4. Open cmd
5. cd into your Debug Folder (eg \SPA_repo\Debug\)
5. Type in "AutoTester C:\yourdirectory\source.txt C:\yourdirectory\queries.txt output.xml"
6. Note that source.txt and queries.txt are test case files stored in AutoTester\tests (eg \SPA_repo\AutoTester\tests\case0)
7. To view the output.xml created, add in analysis.xsl in the same folder as output.xml.(ie in \SPA_repo\Debug\)
8. Use Firefox to open output.xml

analysis.xsl is given in the AutoTester guide pg (http://www.comp.nus.edu.sg/~cs3201/Tools-Lab/AutoTester.html)

****************************
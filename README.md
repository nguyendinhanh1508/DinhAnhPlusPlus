# D++
Dinh Anh's Programming Language<br>
This is an intepreted, high-level programming language that is still WIP.<br>
The language I use to interpret this is C++.<br>
# How to use this language
- create a text file named main.dpp
- compile the main_for_script.cpp file
- then type the command: ./main_for_script.exe main.dpp in terminal
- if you want to run and write the code using the main.cpp file, you have to type RUN for it to start running the code for you
# Language syntax
- ***comments are started with the*** ```#``` ***symbol***
- ***spaces will be ignored, this language doesn't use indentations***
- ***this also doesn't support semicolons, they're only used to separate parameters in the for loop***
- ***output:<br>***
```out```<br>
- ***input:<br>***
```in```<br>
- ***getline(takes in input from the entire line):<br>***
```getline```<br>
- ***end this line and go to next line is:***<br>
```nextline```<br>
- ***it supports multiple operands:<br>***
&nbsp;&nbsp;&nbsp;&nbsp;modulo: ```%```<br>
&nbsp;&nbsp;&nbsp;&nbsp;xor: ```^```<br>
&nbsp;&nbsp;&nbsp;&nbsp;or: ```|```<br>
&nbsp;&nbsp;&nbsp;&nbsp;or(for comparing): ```||```<br>
&nbsp;&nbsp;&nbsp;&nbsp;and: ```&```<br>
&nbsp;&nbsp;&nbsp;&nbsp;and(for comparing): ```&&```<br>
&nbsp;&nbsp;&nbsp;&nbsp;left shift: ```<<```<br>
&nbsp;&nbsp;&nbsp;&nbsp;right shift: ```>>```<br>
&nbsp;&nbsp;&nbsp;&nbsp;greater than: ```>```<br>
&nbsp;&nbsp;&nbsp;&nbsp;greater than or equals to: ```>=```<br>
&nbsp;&nbsp;&nbsp;&nbsp;less than: ```<```<br>
&nbsp;&nbsp;&nbsp;&nbsp;less than or equals to: ```<=```<br>
&nbsp;&nbsp;&nbsp;&nbsp;assign: ```=```<br>
&nbsp;&nbsp;&nbsp;&nbsp;equal to: ```==```<br>
&nbsp;&nbsp;&nbsp;&nbsp;not equals to: ```!=```<br>
&nbsp;&nbsp;&nbsp;&nbsp;addition: ```+```<br>
&nbsp;&nbsp;&nbsp;&nbsp;subtraction: ```-```<br>
&nbsp;&nbsp;&nbsp;&nbsp;multiplication: ```*```<br>
&nbsp;&nbsp;&nbsp;&nbsp;division: ```/```<br>
- ***to declare a variable, we write variable type and then the name:<br>***
```<variable type> <name>```<br>
- ***it also supports a few value types:<br>***
&nbsp;&nbsp;&nbsp;&nbsp;string: ```string```<br>
&nbsp;&nbsp;&nbsp;&nbsp;integer: ```int```<br>
&nbsp;&nbsp;&nbsp;&nbsp;character: ```char```<br>
&nbsp;&nbsp;&nbsp;&nbsp;list: ```list```<br>
&nbsp;&nbsp;&nbsp;&nbsp;boolean: ```bool```<br>
&nbsp;&nbsp;&nbsp;&nbsp;function: ```func```<br>
- ```true``` ***and*** ```false``` ***are converted to 1 and 0***
- ***array elements are accessed using square brackets,*** ```a[0]```
- ***curly brackets are used for function bodies***
- ***list syntax:***<br>
```{<element>, ...}```<br>
- ***string syntax:***<br>
```"<string>"```<br>
- ***char syntax:<br>***
```'<char>'```<br>
- ***function syntax:<br>***
```func <function_name>(<name>, ...){```<br>
```      <function body>```<br>
```      return <value>```<br>
```}```<br>
for functions, if you want to pass by reference you can do ```&<name>```
- ***if statement syntax:<br>***
```if(<parameter>){```<br>
```    <if statement body>```<br>
```}```<br>
- ***for loop syntax:<br>***
```for(<declaration>; <conditions>; <actions>){```<br>
```    <for loop body>```<br>
```}```<br>
- ***while loop syntax:<br>***
```while(<condition>){```<br>
```    <while loop body>```<br>
```}```<br>

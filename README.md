# D++
Dinh Anh's Programming Language
This is an intepreted, high-level programming language that is still WIP.
The language I use to interpret this is C++.
Currently, these are the features of this language:
- calculate
- create variable
- assign value
- input/output
- supports integers, characters, strings, booleans, lists
- you can assign all of the types of values above to a list
- supports parentheses
- supports comparisons
- supports functions
- supports for loops, while loops, if, else
# How to use this language
- create a text file named main.dpp
- compile the main_for_script.cpp file
- then type the command: ./main_for_script.exe main.dpp in terminal
- if you want to run and write the code using the main.cpp file, you have to type RUN for it to start running the code for you
# Language syntax
- comments are started with the '#' symbol
- spaces will be ignored, this language doesn't use indentations
- this also doesn't support semicolons, they're only used to separate parameters in the for loop
- output is ```out```
- input is ```in```
- getline(takes in input from the entire line) is ```getline```
- end this line and go to next line is ```nextline```
- it supports multiple operands:<br>
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
- to declare a variable, we write variable type and then the name:```<variable type> <name>```<br>
- it also supports a few value types:<br>
&nbsp;&nbsp;&nbsp;&nbsp;string: ```string```<br>
&nbsp;&nbsp;&nbsp;&nbsp;integer: ```int```<br>
&nbsp;&nbsp;&nbsp;&nbsp;character: ```char```<br>
&nbsp;&nbsp;&nbsp;&nbsp;list: ```list```<br>
&nbsp;&nbsp;&nbsp;&nbsp;boolean: ```bool```<br>
&nbsp;&nbsp;&nbsp;&nbsp;function: ```func```<br>
- ```true``` and ```false``` are converted to 1 and 0
- array elements are accessed using square brackets, ```a[0]```
- curly brackets are used for function bodies
- function syntax:<br>
```func <function_name>(&<name if you want to pass by reference>, <name>)```<br>
```      <function body>```<br>
```      return <value>```<br>
```}```<br>
- if statement syntax:<br>
```if(<parameter>){```<br>
```    <if statement body```
```}```<br>
- for loop syntax:<br>
```for(<declaration>, <conditions>, <actions>){```
```    <for loop body>```
```}```<br>
- while loop syntax:<br>
```while(<condition>){```
```    <while loop body>```
```}```<br>

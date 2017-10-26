# MAP - Modular Assistant Program

Here's MAP: Modular Assistant Program; a simple program, with complex modules, that assists you in tasks. The program works like a usual assistant, but it has one major difference:
```
You can define how your assistant works
```
It's simple, all you need is to use modules, that you can add, remove, create and modify, as you wich.

## Modules
A module is a program with a **specific** objective. There is two kinds of modules, There is two kinds of modules, the **loop modules**, that repeat a task in a defined time, and the **input modules**, that execute a task when a input is given.

### Loop Modules
Loop modules are usefull when you need something to be executed periodically, like a module to check your email, wether, todo list, and so on. These are simple modules, that execute some really specific task.

### Input Modules
Here's where things become interesanting. A input module is a module that responds to a given input. What MAP does is keep a list of input modules, and when a input is given, it send the input to the first input module in the list. If the first module don't know how to answer the given input, it just pass to the following module. The idea is to keep more specific modules on top of the list, and generic modules on the bottom.

Let's go to a example. Supose that you configurated the manager to have the **DarlaModule** and **CalculatorModule**. Then, you give the following inputs:
```C
42
::@Answer to the Ultimate Question of Life, the Universe, and Everything.
42
```
The **DarlaModule** doesn't know how to answer to the input `42`, so, it leave to the next module. The **CalculatorModule** knows the answer to `42` and gives you `42 = 42.000000`. Then, we send the input `::@Answer to the Ultimate Question of Life, the Universe, and Everything.`, the **DarlaModule** will handle this, it will get the last input and append `Answer to the Ultimate Question of Life, the Universe, and Everything.` as a answer, what is means is that, when **DarlaModule** receive `42`, it returns `Answer to the Ultimate Question of Life, the Universe, and Everything.`. Again, we send `42`, but now **DarlaModule** knows how to answer it, then, it will return `Answer to the Ultimate Question of Life, the Universe, and Everything.`.

What is happening is that **DarlaModule** is a more specific module, that answer to user-created commands, with user-defined answers, so, we expect that the **DarlaModule** doesn't answer to a lot of inputs, just the directly defined by the user. The **CalculatorModule** should handle any mathematical operation, so, it is a more generic module. So, if you put the **CalculatorModule** before **DarlaModule**, you can't make **DarlaModule** "overload" the answer to a input that **CalculatorModule** can handle, because the program just get the answer from the first module that answers. Simple, right?

### Components
There is one more piece. By default, you will have everything working via terminal, the inputs and outputs. But you can imagine that a lot of modules working in a single interface, to input and output, will make a great mess. So, this is where the **components** get in.

**Components** are interfaces to input or output, allowing more control over the modules. By now, the **input modules** shall respond input and output from a single component. But the __loop modules__ already allow setting the **output component** (it doesn't receive input, remember?), that can be the terminal (by default) or any UI created. So, you can use pop-up windows (I really don't recommend this), notifications (this one is good), or any other output interface that you create.

## Standard Modules
#### Darla
**Darla** (Dynamic Appendable Rules Learning Assistant) was originally a independet assistant made with python. Then, it was remade in C++, and modularizated to MAP. It goals is to answer the input using a **set of rules** defined by the user.
#### Calculator
**Calculator** is a module to handle mathematical operations. By now, the module only knows how to **solve simple operations**, but, as all modules, improvements keep being be made.
#### Chatter
**Chatter** is a super generic module, that works as a **chatterbot**, always trying to find a answer to a given input.
#### Reminder
**Reminder** uses a .todo file to reminder taks for the user. You can use [Sublime](https://www.sublimetext.com/) with [PlainTasks](https://github.com/aziz/PlainTasks), or [Atom](https://atom.io/) with [tasks](https://atom.io/packages/tasks) if you want to read the .todo file.
## Standard Components
The default output and input components for any module is the terminal. The following components is by default with MAP, and you can use with any module that you like:
#### Notification
**Notification** Is a output interface, currently only for Linux, that outputs via notifications using the _libnotify_.

## Expansions
You can create your own **modules** and **components**. To do so, you have two options: **C++** or **Python**. 

Is recommended using **C++** to create modules. To do so, you just have to create a `.h` file in the `src/MBS` folder, with a class that extends from `Module`, and override the function `void loop()` or `std::string input(std::string inp)`. In this way, you have full control over the module you are creating, and can make it as much complex as you wish. After created, you need to **include** the module in `map.h`.

To create modules using **Python**, you just have to create a `.py` file in the `src/Python` folder. The file will be something like this:
```Python
def loop():
	print 'Fuction for a looping module!'

def input(inp):
	inp = inp.lower();
	if inp == "is python there?":
		return "Function for the input module!"
	return ""

def destroy():
	print "Function to destroy the module!"
```
You can also create extra functions, classes, or anything you like inside the file. You don't need to declare the function if you won't use it.

## More
To know more about creating **modules** and **components**, or to know about the default modules that comes with MAP, please, use the [wiki page](https://github.com/kiq005/Modular-Assistant-Program/wiki). Any trouble using the program? Found bugs? Has suggestions? Please, use the [issues page](https://github.com/kiq005/Modular-Assistant-Program/issues). In any case, you can also send a e-mail to: [kaique_q@live.com](mailto:kaique_q@live.com). 

## Updates
2017-10-26 : Creating Python modules is easier now. All you need is to create the python file, there is no more need to modify `src/MBS/m_python_test.h` every time you create a new Python module (this means that we get rid of the old auto-class creation, and now use a unique class that the objects represent the modules). Now, the Python module can be a loop and input module at the same time (that was not possible in the old way). Also, `c_notification` and `m_reminder.h` works with more generic messages.

2017-10-23 : Now, the Manager (`MBS.h`) can read commands from the modules. The `$EXIT$` command has been added to the Manager, it allows to exit the program, sending a destroy command for all modules before exit. You can override the `void destroy()` function of the modules directly with C++, with Python you just need to use `PythonDestroy(PythonInput, "p_pytest", "test_destroy");` to call a destroy function [(wiki)](https://github.com/kiq005/Modular-Assistant-Program/wiki/Python). To pass a command to the Manager, the module just have to return the command. The Darla module return the command `$EXIT$` when it receive `::exit` from the user.
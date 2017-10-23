#pragma once

PythonLoopModule(PythonModule, "p_pytest", "loop");
PythonInputModule(PythonInput, "p_pytest", "output");

PythonDestroy(PythonInput, "p_pytest", "test_destroy");
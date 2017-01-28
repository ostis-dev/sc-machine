## v0.3.0 rc1

* Rename module funcitons from `initialize`, `shutdown`, `load_priority` to `sc_module_initialize`, `sc_module_shutdown`, `sc_module_load_priority`
* Update code to codestyle. Most functions was renamed from `functionName` to `FunctionName`. Iterator `value` function replaced with `Get`.
* Implement support of result codes for action agents
* Improve unit test system
* Add [MkDocs](http://www.mkdocs.org/) documentation generator support
* Add `clang-format` config
* Add codestyle document
* Add documentation for `C++` API
* Implement multithreaded emit of sc-events
* Implement ref counter for sc-elements
* Cover whole functionality of `C++` API by unit tests
* Implement `C++` API


Templates is a very powerful mechanism to work with semantic network (graph). You can search and generate any constructions using templates.

## Create template

Templates API provides different ways to create `ScTemplate`:

- [With C++ syntax](#sctemplatebuilder)
- [From SCs-text](#sctemplatescsbuilder)
- [From SC-structure](#sctemplatestructbuilder)

### ScTemplateBuilder

Before reading this paragraph please read common [information about types](el_types.md).

Triple is a main concept of template representation. Each triple is represented by:

- Source `sc-element` is first element of triple;
- `sc-edge` is a second element of triple;
- Target `sc-element` is the third element of a triple.

Each element of triple could be:

- `ScAddr` - is a known `sc-element` (**constant**)
- Variable `ScType` - is unknown `sc-element` that should be found (**variable**)
- Replacement - is an alias of `sc-element` that is previously defined in template (**constant**)

Let's show how to define simple template below with C++ code.

<scg src="../../images/templates/example_template_device.gwf"></scg>

```cpp
ScAddr const deviceAddr = ...;

ScTemplatePtr templ = ScTemplateBuilder()
  .Triple(deviceAddr,
          ScType::EdgeAccessVarPosPerm,
          ScType::NodeVarClass >> "_device") // assign alias to element
  .Triple("_device", // use assigned alias from previous triple
          ScType::EdgeAccessVarPosPerm,
          ScType::NodeVar)
  .Make();
```

### ScTemplateSCsBuilder

This class used to build templates from `SCs-text` representation. API of this class is pretty simple:

```cpp
ScTemplatePtr templ = ScTemplateSCsBuilder(ctx)
  .Make(
    "device _-> _device_class;;"
    "_device_class _-> _device_instance;;"
    "_device_class <- sc_node_class;;"
  );
```

### ScTemplateStructBuilder

Template could be represented with SC-code in memory. It is also possible to build template from this representation.
Assume that this scs-file is already translated into sc-memory:

```scs
[example_template] <= nrel_system_identifier: [*
    device _-> _device_class;;
    _device_class _-> _device_instance;;
  *];;
_device_class <- sc_node_class;;
```

Developer can build template using structure from sc-memory:

```cpp
ScAddr const structAddr = ctx.HelperFindBySystemIdtf("example_template");
ScTemplatePtr templ = ScTemplateSCsBuilder(ctx)
  .Make(structAddr);
```

!!! tip
    `ScTemplateStructBuilder` adds replacements (aliases) using `addr.ToString()` for each added `ScAddr`


## Template parameters

It is possible to use parameters in template search/generation. Please use `ScTemplateParams` class for that.


## Search by template

Search engine traverses sc-memory to find all contructions that correspond to template. Let's assume that we need to check if specified sc-element (`_device`) is included into `device` set and `device_enabled` set:

<scg src="../../images/templates/template_example_2.gwf"></scg>

**Code** generates equal template:

```cpp
ScAddr const device_addr = ctx.HelperFindBySystemIdtf("device");
ScAddr const device_enabled_addr = ctx.HelperFindBySystemIdtf("device_enabled");

ScTemplatePtr templ = ScTemplateBuilder()
  .Triple(
    device_addr,    // sc-addr of device node
    ScType::EDGE_ACCESS_VAR_POS_PERM,
    ScType::NODE_VAR >> "_device_instance")
  .Triple(
    device_enabled_addr,    // sc-addr of device_enabled node
    ScType::EDGE_ACCESS_VAR_POS_PERM,
    "_device_instance")
  .Make();

ScTemplateSearch search(ctx, *templ);
// Iterate all results
for (ScTemplateNamedStruct const & res : search)
{
  ScAddr const instanceAddr = res["_device_instance"];
  // Do something with found addr
}
```

Search algorithm trying to find all possible variants of specified construction where _constants_ corresponds to _variables_ from template . It use any constants (available `ScAddr`'s from parameters to find equal constructions in sc-memory).

It is possible to run search with parameters:

```cpp
...
ScTemplateParams params;
params.Add("_device_instance", instanceAddr); // element _device_instance in template become known

ScTemplateSearch search(ctx, *templ, params);
...
```

## Generate by template

It is possible to generate construction by `ScTemplate`:

```cpp
ScAddr const device_addr = ctx.HelperFindBySystemIdtf("device");
ScAddr const device_enabled_addr = ctx.HelperFindBySystemIdtf("device_enabled");

ScTemplatePtr templ = ScTemplateBuilder()
  .Triple(
    device_addr,    // sc-addr of device node
    ScType::EDGE_ACCESS_VAR_POS_PERM,
    ScType::NODE_VAR >> "_device_instance")
  .Triple(
    device_enabled_addr,    // sc-addr of device_enabled node
    ScType::EDGE_ACCESS_VAR_POS_PERM,
    "_device_instance")
  .Make();

ScAddr const deviceInstance = ctx.CreateNode(ScType::NodeConstMaterial);

ScTemplateParams params;
params.Add("_device_instance", deviceInstance); // element _device_instance in template become known

ScTemplateGenerate generator(ctx, *templ);
ScTemplateGenerate::Result result = generator.Do();

if (result)
{
  // *(result["_device_instance"]) == deviceInstance
}

```

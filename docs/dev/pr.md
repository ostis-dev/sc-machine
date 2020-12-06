This page describes rules to create Pull Requests.
### Commits
Each commit message should be formed as: `[tag1]...[tagN] Message text (#issue)`

Message text should start from an upper case letter. If commit doesn't fix or implement any `#issue`, then it shouldn't be pointed in commit message
  Examples:
<pre>
[cpp] Colored log output
[cpp][test] Add unit test for ScEvent class
[kpm][search] Relation type check added
</pre>

Possible tags:

  * `[builder]` - changes in builder tool
  * `[changelog]` - update changelog
  * `[cmake]` - changes in cmake build system
  * `[codegen]` - changes in code generator
  * `[core]` - changes in `sc-storage` code
  * `[doc]` or `[docs]` - update documentation
  * `[git]` - changes in git configuration
  * `[python]` - changes in python modules
  * `[sc-kpm]` - changes in `sc-kpm` code
  * `[sc-memory]` - changes in `sc-memory` code
  * `[sc-server]` - changes in `sc-server` code
  * `[sc-network]` - changes in `sc-network` code
  * `[tests]` or `[test]` - changes in tests
  * `[thirdparty]` - updated thirdparty libraries
  * `[web]` - changes in web interface

***
Each commit in Pull request should be an atomic. Another word implement or fix one feature. For example:
<pre>
Lat commit
...
[cpp] Colored log output
[cpp] Add class to work with console
...
Init commit
</pre>

 In this example we add class to work with console (where implemented colored output), then **in another commit** we had implementation of colored log output

***
Each commit should have not much differences excluding cases, with:

  * CodeStyle changes;
  * Renames;
  * Code formating;

**Do atomic commits for each changes.** For example if you rename some members in `ClassX` and `ClassY`, then do two commits:
<pre>
[refactor] Rename members in ClassX according to codestyle
[refactor] Rename members in ClassY according to codestyle
</pre>

**Do not mix codestyle changes and any logical fixes in one commit**

***
_**All commit, that not applies to this rules, should be split by this rules. Another way they will be rejected with Pull request.**_
### Pull request
Each pull request with many changes, that not possible to review (excluding codestyle, rename changes), will be rejected.
***
For each pull request you should do:

  * update `docs/changelog.md` file according to your commits
  * ensure prepare updates for documentation
  * cover new functionality [by tests](test.md)
  * your code should be written according to a [codestyle](codestyle.md)

***
After prepare pull request an automation tests will run on it. If any test doesn't passed, then pull request couldn't be merged and you should to fix it.
***
All review or other fixes to pull request should be appended as new commits to it

# Workspace

A workspace is the common ground that ties the packages in this project together. Some examples of commonality:

- When a change in one will usually introduce a breaking change in another (tight coupling)
- Documentation that is coupled to the version
- Tooling sub-projects
- Code for testing

If they do not have this commonality, they should likely be moved into separate projects.

## Setup

### Dune

First, we'll start with creating a dune-workspace file. This will define our a default context for the
builder. It's not necessary yet, as we're only using default options.

```shell
cd $WORKSPACE
echo -e "(lang dune 2.7)\n(context default)" > $WORKSPACE/dune-workspace```

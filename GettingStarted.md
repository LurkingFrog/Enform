# Getting Started with PPX

A walkthrough to create a new ReasonML/ReScript PPX project from the ground up.

The docs were thin and scattered, and required a bunch of trial and error when I started working on
EnformDerive so I wanted to document it all for next time.

## Assumptions

Since I'm writing this based on my workflow, the tools are going to be specific to my experience. This includes
things like coding with ReScript/ReasonML instead of OCaml, SVN with Github, and using VS Code as my IDE.

I'm also using the Reason syntax instead of ReScript because the tooling is not quite there yet.

## The Environment

This is being written based on my [current environment](https://github.com/LurkingFrog/dots) built in an Ubuntu
20.04.1 LTS (Focal Fossa) virtual machine. This is a subset of tools that I use.

I'm writing this because I always get iklled when I find a "project template" which doesn't quite work and I
have to figure out everything just to make the one change I need.

1. Install m4 (required by esy for building a OCaml compiler) ```sudo apt install -y m4```
2. Install git
   ```sudo apt install -y git```
3. Install npm/nvm

   - Install a versioned instance of node. [nvm docs](https://github.com/nvm-sh/nvm#install--update-script)
     ```curl -o- https://raw.githubusercontent.com/nvm-sh/nvm/v0.36.0/install.sh | bash```
   - And make sure it starts in your shell - add the following to your .zshrc/.bashrc

     ```export NVM_DIR="$([ -z "${XDG_CONFIG_HOME-}" ] && printf %s "${HOME}/.nvm" || printf %s "${XDG_CONFIG_HOME}/nvm")"
        [ -s "$NVM_DIR/nvm.sh" ] && \. "$NVM_DIR/nvm.sh" # This loads nvm```

4. Install Opam
   The oCaml package manager. The official install docs are at [docs](https://opam.ocaml.org/doc/Install.html).

   - Install the latest version
     ```sh <(curl -sL https://raw.githubusercontent.com/ocaml/opam/master/shell/install.sh)```
   - Initialize Opam
     ```opam init```

5. Install esy
   A simplified workflow engine for common development actions. [docs](https://esy.sh/docs/)
6. Install Dune
   This is the software required to build the packages. [docs](https://dune.readthedocs.io/en/stable/index.html)
   ```opam install dune```
7. Install the OCaml language server (code completion, formatting, etc)
   ```opam pin add ocaml-lsp-server https://github.com/ocaml/ocaml-lsp.git```

8. Install extensions in VS Code (or your IDE of choice)
   - *maelvalais.dune* The dune processor

## Initializing the project "DemoPPX"

My preferred layout is to have all the associated packages in one repo to maintain a standard versioning, but
allow for components to each be installed independently as needed.

For a derive ppx, I have a minimum of four packages:

- *Base* This contains the primitives and helper functions. Separating this from the derive allows users to
  code their own custom implementations when the derive doesn't work.
- *Derive* This specifically is limited to actually parsing the AST and generating the derived functions.
  Nothing from this package should be exported except for the Derived functions.
- *Tests* All the functional and integration testing should be contained here
- *Examples* A set of complete projects that showcase the Base and Derive packages. These can also double as
  top level integration tests.

I will refer to the root folder of this project as **WORKDIR**

1. Create a repo on github.com [instructions](https://docs.github.com/en/free-pro-team@latest/github/getting-started-with-github/create-a-repo)
2. Git clone the new repo into a directory and then switch into it.
3. Update the .gitignore with:

   ```shell

   # The directory where temporary and final files are placed
   build

   # per package build files
   _esy


   # Installed Javascript modules from npm
   node_modules
   ```

4. Create an esy.json file for the full project in **WORKDIR**

   ```json

   ```

5. For each package, do steps 5-
6. Create an package.json with the command ```npm init```
   This is primarily a placeholder as we are going to be using esy.json for most of the management functions
7.

After completing all the initialization, your directory structure should look like this:

generated using tree (see git alias and script [current environment](https://github.com/LurkingFrog/dots/bin/tree.sh))

DemoPPX
├── packages
│   └── enform-derive
│       └── package.json
├── dune-project
├── GettingStarted.md
├── LICENSE
└── README.md

## At last, the code begins


### Sources

These are some of the demos/tutorials/projects I used for information.

- [VSCode OCaml Platform](https://github.com/ocamllabs/vscode-ocaml-platform)
- (https://github.com/esy-ocaml/hello-reason)
- (https://sookocheff.com/post/ocaml/starting-a-new-ocaml-project-using-dune-and-visual-studio-code/)

# NPM/NVM

Install npm/nvm

- Install a versioned instance of node. [nvm docs](https://github.com/nvm-sh/nvm#install--update-script)
  ```curl -o- https://raw.githubusercontent.com/nvm-sh/nvm/v0.36.0/install.sh | bash```
- And make sure it starts in your shell - add the following to your .zshrc/.bashrc

  ```export NVM_DIR="$([ -z "${XDG_CONFIG_HOME-}" ] && printf %s "${HOME}/.nvm" || printf %s "${XDG_CONFIG_HOME}/nvm")"
    [ -s "$NVM_DIR/nvm.sh" ] && \. "$NVM_DIR/nvm.sh" # This loads nvm```

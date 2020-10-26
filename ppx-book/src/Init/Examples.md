# Examples

This is where I usually start my projects. The example is where I think about how the code will actually be
used in the real world. Too often, a developer gets lost in their own head writing a project and what seems
easy through familiarity is actually an inscrutable mess to everybody else.

An example should be complex enough to showcase how the library/project is used for a real test case.
As an end user, "Hello World" is almost never useful in showing the how and why it is used.

## Setting up the configuration files

In the case of Enform, we want to set up a basic ReasonReact project with hot reloading via WebPack. While a
bit complicated, it will not only be easier for us to work with, it will encourage others to be able to
contribute.

For this, we will be creating the SubmissionForm project

### BuckleScript/ReScript

**NOTE** BuckleScript is being rebranded as ReScript as of this writing. Unfortunately, the tooling on ReScript
is not quite ready yet, so this will be setup as the former.

bsconfig.json

```json
{
  "name": "enform-example",
  "reason": {
    "react-jsx": 3
  },
  "sources": {
    "dir" : "src",
    "subdirs" : true
  },
  "bsc-flags": ["-bs-super-errors", "-bs-no-version-header"],
  "package-specs": [{
    "module": "commonjs",
    "in-source": true
  }],
  "suffix": ".bs.js",
  "namespace": true,
  "bs-dependencies": [
    "reason-react",
    "reason-uuid",
  ],
  "refmt": 3,
  "ppx-flags": [
  ],
  "warnings": {
    "number": "+A-42-48-4",
    "error": "+A-3-44-102-103"
  }
}
```

### Webpack

dev.webpack.json

```json
const path = require("path");
const HtmlWebpackPlugin = require("html-webpack-plugin");
const ExtractCssChunks = require("extract-css-chunks-webpack-plugin");

const outputDir = path.join(__dirname, "build/");
const isProd = process.env.NODE_ENV === "production";

const ASSET_PATH = process.env.ASSET_PATH || "/";

module.exports = {
  entry: "./src/Index.bs.js",
  mode: isProd ? "production" : "development",
  devtool: "inline-source-map",
  output: {
    path: outputDir,
    publicPath: ASSET_PATH,
    filename: "Index.js",
  },
  plugins: [
    new HtmlWebpackPlugin({
      template: "src/index.html",
      inject: false,
    }),
    new ExtractCssChunks(),
  ],
  devServer: {
    compress: true,
    hot: true,
    contentBase: outputDir,
    host: process.env.HOST || "0.0.0.0",
    port: process.env.PORT || 9080,
    historyApiFallback: true,
  },
  module: {
    rules: [
      {
        test: /\.css$/,
        use: [
          { loader: ExtractCssChunks.loader },
          // { loader: "style-loader" },
          { loader: "css-loader", options: { sourceMap: true } },
        ],
      },
      {
        test: /\.(png|svg|jpg|gif)$/,
        loader: "file-loader",
      },

      {
        test: /\.(woff|woff2|eot|ttf|otf)$/,
        loader: "file-loader",
      },
    ],
  },
};

```

### Package.json

```json
{
  "name": "enform-example",
  "version": "0.1.0",
  "scripts": {
    "build": "bsb -make-world",
    "start": "bsb -make-world -w < /dev/zero",
    "clean": "bsb -clean-world",
    "webpack": "webpack -w",
    "webpack:production": "NODE_ENV=production webpack",
    "server": "webpack-dev-server",
    "test": "echo \"Error: no test specified\" && exit 1"
  },
  "keywords": [
    "BuckleScript",
    "ReasonReact",
    "reason-react"
  ],
  "author": "",
  "license": "MIT",
}
```

### Esy.json

```json

```

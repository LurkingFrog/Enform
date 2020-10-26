const path = require("path");
const HtmlWebpackPlugin = require("html-webpack-plugin");
const ExtractCssChunks = require("extract-css-chunks-webpack-plugin");

const outputDir = path.join(__dirname, "build/");
console.log("The outputDir is " + outputDir);
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
      template: path.resolve(__dirname, "src", "index.html"),
      inject: false,
    }),
    new ExtractCssChunks(),
  ],
  devServer: {
    compress: true,
    hot: true,
    contentBase: outputDir,
    host: process.env.HOST || "0.0.0.0",
    port: process.env.PORT || 9085,
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

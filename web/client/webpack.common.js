const webpack = require('webpack');
const path = require('path');
const copy = require("copy-webpack-plugin");

const outputPath = path.resolve(__dirname, 'assets');

module.exports = {
  mode: 'none',
  entry: {
    app: './src/index.tsx'
  },
  module: {
    rules: [
      {
        test: /\.tsx?$/,
        use: 'ts-loader',
        exclude: /node_modules/
      },
      {
        test: /\.css$/,
        use: [ 'style-loader', 'css-loader' ]
      },
      {
        test: /\.(woff(2)?|ttf|eot|svg|png)(\?v=\d+\.\d+\.\d+)?$/,
        use: [
          {
            loader: 'file-loader',
            options: {
              name: '[name].[ext]',
              outputPath: 'fonts/'
            }
          }
        ]
      }
    ]
  },
  plugins: [
    new webpack.DefinePlugin({
      'process.env.NODE_ENV': JSON.stringify('production')
    }),
    new copy({
      patterns: [
        { from: 'node_modules/@ostis/scs-js-editor/build/editor.worker.js', to: outputPath + '/editor.worker.js' }
      ]
    })
  ],
  resolve: {
    extensions: [ '.tsx', '.ts', '.js', '.css' ]
  },
  externalsPresets: { node: true },
  output: {
    filename: 'sc-memory-web.js',
    path: outputPath,
    libraryTarget: 'umd',
    library: 'ScMemoryWeb'
  }
};
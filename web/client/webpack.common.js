const MonacoWebpackPlugin = require('monaco-editor-webpack-plugin');
const webpack = require('webpack');
const { CleanWebpackPlugin } = require('clean-webpack-plugin');
const path = require('path');

const outputPath = path.resolve(__dirname, 'build');

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
    new MonacoWebpackPlugin({
      "languages": [],
      "features": [
        'bracketMatching', 'caretOperations', 'clipboard', 'codelens', 'colorDetector', 'comment', 'contextmenu',
        'coreCommands', 'cursorUndo', 'find', 'folding', 'format', 'gotoLine', 'hover', 'inPlaceReplace', 'inspectTokens', 'linesOperations', 'links',
        'parameterHints', 'rename', 'smartSelect', 'snippets', 'suggest', 'wordHighlighter', 'wordOperations',
      ]
    }),
    new CleanWebpackPlugin(),
    new webpack.DefinePlugin({
      'process.env.NODE_ENV': JSON.stringify('development')
    }),
  ],
  resolve: {
    extensions: [ '.tsx', '.ts', '.js', '.css' ]
  },
  externalsPresets: { node: true },
  output: {
    filename: 'bundle.js',
    path: outputPath,
    libraryTarget: 'var',
    library: 'SCsEditor'
  }
};
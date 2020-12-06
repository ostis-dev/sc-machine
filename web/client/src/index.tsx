
import * as React from 'react';
import * as ReactDOM from 'react-dom';
import { Provider } from 'react-redux';
import { BrowserRouter } from 'react-router-dom';

import { configureStore } from './store';
import {AppContainer } from './App';

import { SCsInitGlobal } from '@ostis/scs-js-editor';

SCsInitGlobal();

const store: any = configureStore();

ReactDOM.render(
  <Provider store={store}>
    <BrowserRouter>
      <AppContainer />
    </BrowserRouter>
  </Provider>,
  document.getElementById("content")
);

import * as React from 'react';
import * as ReactDOM from 'react-dom';
import * as redux from 'redux';
import { Provider } from 'react-redux';
import { BrowserRouter } from 'react-router-dom';

import { configureStore, Store } from './store';
import { AppContainer } from './App';

import { SCsInitGlobal } from '@ostis/scs-js-editor';

import { ServerRoot } from './utils/server';

SCsInitGlobal();

const store: redux.Store<Store> = configureStore();
const server: ServerRoot = new ServerRoot('ws://localhost:8090/ws_json', store);
server.Start();

ReactDOM.render(
  <Provider store={store}>
    <BrowserRouter>
      <AppContainer />
    </BrowserRouter>
  </Provider>,
  document.getElementById("content")
);
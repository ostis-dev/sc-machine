import { combineReducers } from 'redux';

import * as ui from './uiReducers';
import * as net from './netReducer';
import * as services from './serviceReducers';

export const rootReducer = combineReducers({
  ui: ui.reducer,
  net: net.reducer,
  services: services.reducer,
});
import * as redux from 'redux';
import { composeWithDevTools } from 'redux-devtools-extension';

import { rootReducer } from './reducers/rootReducer';
import { storeInitialState, Store } from './store';

const composeEnhancers = composeWithDevTools({
    
});

export function configureStore() : redux.Store<Store> {
  return redux.createStore(rootReducer, storeInitialState, composeEnhancers()) as redux.Store<Store>;
}
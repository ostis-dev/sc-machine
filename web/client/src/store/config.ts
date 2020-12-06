import { createStore, Store } from 'redux';
import { composeWithDevTools } from 'redux-devtools-extension';

import { rootReducer } from './reducers/rootReducer';
import { storeInitialState } from './store';

const composeEnhancers = composeWithDevTools({
    
});

export function configureStore() : Store<{}> {
  return createStore(rootReducer, storeInitialState as any, composeEnhancers());
}
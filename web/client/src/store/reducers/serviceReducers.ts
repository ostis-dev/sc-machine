import * as store from '../store';
import { Action } from '../actions/baseAction';
import * as serviceAction from '../actions/serviceActions';

export function reducer(state: store.Services = store.servicesInit, action: Action<any>) {

  switch (action.type) {

    case serviceAction.Type.Init:
      return action.payload;

    default:
      return state;
  }
}

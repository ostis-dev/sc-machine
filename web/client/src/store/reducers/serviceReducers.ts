import * as serv from '../interfaces/services';
import { Action } from '../actions/baseAction';
import * as serviceAction from '../actions/serviceActions';

export function reducer(state: serv.Services = serv._initServices, action: Action<any>) {

  switch (action.type) {

    case serviceAction.Type.Init:
      return action.payload;

    default:
      return state;
  }
}

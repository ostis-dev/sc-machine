import * as store from '../store';
import { Action } from '../actions/baseAction';
import * as netAction from '../actions/netActions';

export function reducer(state: store.NetworkState = store.NetworkState.Disconnected, action: Action<any>) {

  switch (action.type) {

    case netAction.Type.ChangeState:
      return action.payload;

    default:
      return state;
  }
}

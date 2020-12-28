import * as net from '../interfaces/net';
import { Action } from '../actions/baseAction';
import * as netAction from '../actions/netActions';

export function reducer(state: net.State = net.State.Disconnected, action: Action<any>) {

  switch (action.type) {

    case netAction.Type.ChangeState:
      return action.payload;

    default:
      return state;
  }
}

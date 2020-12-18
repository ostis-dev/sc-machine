import { Action } from './baseAction';
import * as net from '../interfaces/net';

export namespace Type {
  export const ChangeState = "NET_CHANGE_STATE";
}

export function changeNetState(newState: net.State) : Action<net.State> {
  return {
    type: Type.ChangeState,
      payload: newState
  };
}
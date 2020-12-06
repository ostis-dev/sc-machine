import { Action } from './baseAction';
import { NetworkState } from '../store';

export namespace Type {
  export const ChangeState = "NET_CHANGE_STATE";
}

export function changeNetState(newState: NetworkState) : Action<NetworkState> {
  return {
    type: Type.ChangeState,
      payload: newState
  };
}
import { Action } from './baseAction';
import { NetworkState } from '../store';

export namespace Type {
  export const ChangeState = "NET_CHANGE_STATE";
}

export type ChangeNetState = { state: NetworkState };

export function changeNetState(newState: NetworkState) : Action<ChangeNetState> {
  return {
    type: Type.ChangeState,
      payload: {
        state: newState
      }
  };
}
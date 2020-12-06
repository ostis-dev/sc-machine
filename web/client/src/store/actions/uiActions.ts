import { Action } from './baseAction';
import { UIMode, UIState } from '../store';

export namespace Type {
  export const ChangeMode = 'UI_CHANGE_MODE';
  export const ChangeInitMessage = 'UI_CHANGE_INIT_MSG';
}

export function ChangeUIMode(mode: UIMode): Action<UIMode> {
  return {
    type: Type.ChangeMode,
    payload: mode
  };
}

export function ChangeInitMessage(msg: string): Action<string> {
  return {
    type: Type.ChangeInitMessage,
    payload: msg
  };
}
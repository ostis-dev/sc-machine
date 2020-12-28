import { Action } from './baseAction';
import * as ui from '../interfaces/ui';
import * as types from '../interfaces/types';

export namespace Type {
  export const ChangeMode = 'UI_CHANGE_MODE';
  export const ChangeInitMessage = 'UI_CHANGE_INIT_MSG';
  export const ChangeKBOperation = 'UI_KB_CHANGE_OPERATION';
  export const SetKBViewConstruction = 'UI_KB_SET_VIEW_CONSTRUCTION';
}

export function ChangeUIMode(mode: ui.Mode): Action<ui.Mode> {
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

export function ChangeKBOperation(operation: ui.RunningOperation): Action<ui.RunningOperation> {
  return {
    type: Type.ChangeKBOperation,
    payload: operation
  }
}

export function SetKBViewConstruction(construction: types.ScConstruction): Action<types.ScConstruction> {
  return {
    type: Type.SetKBViewConstruction,
    payload: construction
  }
}
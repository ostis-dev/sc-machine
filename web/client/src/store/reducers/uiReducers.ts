import * as store from '../store';
import { Action } from '../actions/baseAction';
import * as uiAction from '../actions/uiActions';

export function reducer(state: store.UIState = store.uiInit, action: Action<any>) {

  switch (action.type) {

    case uiAction.Type.ChangeMode:
      return { ...state, mode: action.payload };

    case uiAction.Type.ChangeInitMessage:
      return { ...state, initMessage: action.payload };

    default:
      return state;
  }
}

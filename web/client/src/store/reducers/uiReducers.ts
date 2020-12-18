import * as ui from '../interfaces/ui';
import { Action } from '../actions/baseAction';
import * as uiAction from '../actions/uiActions';

export function reducer(state: ui.State = ui._initState, action: Action<any>) {

  switch (action.type) {

    case uiAction.Type.ChangeMode:
      return { ...state, mode: action.payload };

    case uiAction.Type.ChangeInitMessage:
      return { ...state, initMessage: action.payload };

    case uiAction.Type.ChangeKBOperation:
      return { 
        ...state, 
        kb: {
          ...state.kb,
          running: action.payload 
        }
      };

    case uiAction.Type.SetKBViewConstruction:
      return {
        ...state,
        kb: {
          ...state.kb,
          construction: action.payload
        }
      }

    default:
      return state;
  }
}

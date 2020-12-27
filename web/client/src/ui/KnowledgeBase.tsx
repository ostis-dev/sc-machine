import * as React from "react";
import { BasePage } from './BasePage';
import { connect, ConnectedProps } from 'react-redux';

import * as store from '../store';

import { SCsEditor } from './editors/SCsEditor';
import { SCgViewer } from './editors/SCgViewer';

import { SCgStruct } from '@ostis/scg-js-editor';

import { RoundButton } from './components/kb/RoundButton';
import { ScTemplateSearchResult } from '@ostis/sc-core';

import * as mui from '@material-ui/core';

export interface KnowledgeBaseProps {
  net: store.net.State,
  ui: store.ui.kb.State,
  services: store.serv.Services,

  // --- Declaration of dispatch
  dispatch: (any) => void,
  dispatchRunning: (operation: store.ui.RunningOperation) => void,
  dispatchSearchResult: (result: ScTemplateSearchResult) => void,
}

export interface KnowledgeBaseState {
  constr: SCgStruct,
}

function mapStateToProps(state: store.Store): any {
  return {
    net: state.net,
    ui: state.ui.kb,
    services: state.services,
  };
}

const mapDispatch = {
  dispatchRunning: (operation: store.ui.RunningOperation) => {
    return store.actions.ui.ChangeKBOperation(operation);
  },
  dispatchSearchResult: (result: ScTemplateSearchResult) => {
    return store.actions.ui.SetKBViewConstruction(store.types.CreateScConstruction(result));
  },
};

class KnowledgeBaseImpl extends React.Component<KnowledgeBaseProps, KnowledgeBaseState> {

  private scsEditorRef: React.RefObject<SCsEditor> = React.createRef<SCsEditor>();
  readonly state: KnowledgeBaseState = {
    constr: null,
  };

  private doSearch() {
    this.props.dispatchRunning(store.ui.RunningOperation.Search);
    this.props.services.sc.templates.DoSearch(this.scsEditorRef.current.content).then((result: ScTemplateSearchResult) => {
      this.props.dispatchSearchResult(result);
      if (result.length > 0) {
        this.props.services.sc.templates.MakeSCgStruct(result).then((constr: SCgStruct) => {
          this.setState({ constr: constr });
        });
      }
    }).finally(() => {
      this.props.dispatchRunning(store.ui.RunningOperation.None);
    });
  }

  private doGenerate() {
    this.props.dispatchRunning(store.ui.RunningOperation.Generate);
  }

  private isButtonDisabled(operation: store.ui.RunningOperation) {
    const isValidOperation: boolean =
      (this.props.ui.running === store.ui.RunningOperation.None || this.props.ui.running === operation);

    const isConnected: boolean = (this.props.net === store.net.State.Connected);

    return (!isConnected || !isValidOperation);
  }

  render() {
    return (
      <BasePage title='Knowledge Base'>
        <mui.Grid container spacing={1} className="kb-editors-container">
          <mui.Grid item xs={12} className='kb-toolbar'>
            <RoundButton
              type='search'
              disabled={this.isButtonDisabled(store.ui.RunningOperation.Search)}
              onClick={this.doSearch.bind(this)}
              running={this.props.ui.running === store.ui.RunningOperation.Search} />
            <RoundButton
              type='generate'
              disabled={this.isButtonDisabled(store.ui.RunningOperation.Generate)}
              onClick={this.doGenerate.bind(this)}
              running={this.props.ui.running === store.ui.RunningOperation.Generate} />
          </mui.Grid>
          <mui.Grid item xs={12} className='kb-toolbar'>
            {/* SCgToolbar */}
          </mui.Grid>
          <mui.Grid item xs={6}>
            <SCsEditor ref={this.scsEditorRef} />
          </mui.Grid>
          <mui.Grid item xs={6}>
            <SCgViewer constr={this.state.constr} />
          </mui.Grid>
        </mui.Grid>
      </BasePage>
    );
  }
}

export const KnowledgeBase = connect(
  mapStateToProps, mapDispatch
)(KnowledgeBaseImpl);
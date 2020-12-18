import * as React from "react";
import * as scg from '@ostis/scg-js-editor';

export interface SCgViewerProps {
  constr: scg.SCgStruct,
};

export interface SCgViewerState {
  viewer: scg.SCgViewer,
  currentConstr: scg.SCgStruct,
};

export class SCgViewer extends React.Component<SCgViewerProps, SCgViewerState> {

  componentDidMount() {
    this.setState({
      viewer: new scg.SCgViewer('scg-container'),
      currentConstr: null,
    });
  }

  componentDidUpdate() {
    if (this.props.constr && (this.props.constr !== this.state.currentConstr)) {
      this.state.viewer.SetStruct(this.props.constr);
      this.props.constr.Update();
      this.state.viewer.Layout();
      this.setState({ currentConstr: this.props.constr });
    }
  }

  render() {
    return (
      <div id="scg-container" className="kb-editor-scg"></div>
    );
  }
};
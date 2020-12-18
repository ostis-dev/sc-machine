import * as React from "react";
import * as scs from '@ostis/scs-js-editor';

export interface SCsEditorProps {
};

export interface SCsEditorState {
  editor: scs.SCsEditor
};


export class SCsEditor extends React.Component<SCsEditorProps, SCsEditorState> {

  public get content(): string {
    return this.state.editor.content;
  }

  componentDidMount() {
    this.setState({
      editor: new scs.SCsEditor(document.getElementById('scs-container'))
    });
  }

  render() {

    return (
      <div id="scs-container" className="kb-editor-scs"></div>
    );
  }
};
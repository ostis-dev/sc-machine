import * as React from "react";
import * as mui from "@material-ui/core";
import * as icons from '@material-ui/icons';

export interface RoundButtonProps {
  type: 'search' | 'generate',
  // Absolute position from top
  disabled?: boolean,
  running?: boolean,
  onClick: () => void,
};

export interface RoundButtonState {

};

export class RoundButton extends React.Component<RoundButtonProps, RoundButtonState> {

  getIcon() {
    if (this.props.type === 'search') {
      return (<icons.SearchRounded />);
    }
    else if (this.props.type === 'generate') {
      return (<icons.CloudUploadRounded />);
    }
  }

  render() {

    let classes: string = 'kb-round-button ' + this.props.type;
    if (this.props.disabled) {
      classes += ' disabled';
    }
    else if (this.props.running) {
      classes += ' running';
    }

    return (
      <mui.IconButton
        className={classes}
        onClick={this.props.onClick}
        size='medium'
      >
        { this.getIcon()}
      </mui.IconButton >
    );
  }
}
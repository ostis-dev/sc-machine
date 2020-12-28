import * as React from "react";

import * as ui from '@material-ui/core';
import { NavLink } from "react-router-dom";

interface DrawerItemProps {
  title: string,
  icon: string,
  to: string,
}

export class DrawerItem extends React.Component<DrawerItemProps> {

  render() {  

    return (
      <NavLink to={this.props.to}>
        <ui.ListItem button
          key={this.props.title}
          classes={{ button: 'button' }}>
          <ui.ListItemIcon classes={{ root: 'item-icon' }}>
            <ui.Icon color='primary' classes={{ root: 'icon' }}>{this.props.icon}</ui.Icon>
          </ui.ListItemIcon>
          <ui.ListItemText primary={this.props.title} classes={{ primary: 'text'}}/>
        </ui.ListItem>
      </NavLink>
    );
  }
}
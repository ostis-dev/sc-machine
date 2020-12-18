import * as React from "react";
import * as mui from "@material-ui/core";

export interface BasePageProps {
  title: string,
};

export class BasePage extends React.Component<BasePageProps, null> {

  render() {
    return (
      <mui.Container className="base-page">
        <mui.Typography variant="h4" gutterBottom>{this.props.title}</mui.Typography>
        {this.props.children}
      </mui.Container>
    );
  }
}
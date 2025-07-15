name: 🐛 Bug Report
description: File a bug in the NymyaOS system or packages
title: "[Bug] "
labels: [bug]
body:
  - type: textarea
    id: what-happened
    attributes:
      label: What happened?
      placeholder: Describe the problem in detail.
    validations:
      required: true
  - type: input
    id: version
    attributes:
      label: Version
      placeholder: "e.g., 0.1.0"
  - type: dropdown
    id: component
    attributes:
      label: Component
      options:
        - nymya-core
        - nymya-runtime
        - nymya-algs
        - installer / ISO
        - GUI
        - Other

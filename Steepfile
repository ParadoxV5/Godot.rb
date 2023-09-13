target :lib do
  check 'lib'
  signature 'sig'
  
  D = Steep::Diagnostic::Ruby
  configure_code_diagnostics(D.strict) do|d|
    d[D::NoMethod] = nil
    d[D::UnknownConstant] = nil
  end
end

target :lib do
  check 'lib'
  signature 'sig'
  
  D = Steep::Diagnostic::Ruby
  configure_code_diagnostics(D.all_error) do|d|
    d[D::FallbackAny] = :information
    d[D::MethodDefinitionMissing] = nil
    d[D::NoMethod] = nil
    d[D::UnknownConstant] = nil
  end
end
